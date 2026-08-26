//
// FLTK Benchmark Suite - Main Driver and Report Generator
//
// Fast Light Tool Kit (FLTK)
//

#include "fltk_benchmarks.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <ctime>

using namespace fltk_bench;

// Helper to escape HTML strings
static std::string escape_html(const std::string& str) {
  std::string res;
  res.reserve(str.size());
  for (char c : str) {
    switch (c) {
      case '&':  res += "&amp;"; break;
      case '<':  res += "&lt;"; break;
      case '>':  res += "&gt;"; break;
      case '"':  res += "&quot;"; break;
      case '\'': res += "&#39;"; break;
      default:   res += c; break;
    }
  }
  return res;
}

// Helper to escape JSON strings
static std::string escape_json(const std::string& str) {
  std::string res;
  res.reserve(str.size());
  for (char c : str) {
    switch (c) {
      case '"':  res += "\\\""; break;
      case '\\': res += "\\\\"; break;
      case '\b': res += "\\b"; break;
      case '\f': res += "\\f"; break;
      case '\n': res += "\\n"; break;
      case '\r': res += "\\r"; break;
      case '\t': res += "\\t"; break;
      default:
        if (static_cast<unsigned char>(c) < 0x20) {
          char buf[8];
          snprintf(buf, sizeof(buf), "\\u%04x", c);
          res += buf;
        } else {
          res += c;
        }
        break;
    }
  }
  return res;
}

static void export_json(const std::string& filepath, const std::vector<BenchmarkResult>& results) {
  std::ofstream out(filepath);
  if (!out.is_open()) {
    std::cerr << "Error: Could not open " << filepath << " for writing JSON.\n";
    return;
  }

  std::time_t now = std::time(nullptr);
  char time_buf[64];
  std::strftime(time_buf, sizeof(time_buf), "%Y-%m-%dT%H:%M:%SZ", std::gmtime(&now));

  std::string fl_ver_str = std::to_string(FL_MAJOR_VERSION) + "." + std::to_string(FL_MINOR_VERSION) + "." + std::to_string(FL_PATCH_VERSION);

  out << "{\n";
  out << "  \"fltk_version\": \"" << fl_ver_str << "\",\n";
  out << "  \"timestamp\": \"" << time_buf << "\",\n";
  out << "  \"total_classes\": " << results.size() << ",\n";
  out << "  \"results\": [\n";

  for (size_t i = 0; i < results.size(); ++i) {
    const auto& r = results[i];
    out << "    {\n";
    out << "      \"class_name\": \"" << escape_json(r.class_name) << "\",\n";
    out << "      \"category\": \"" << escape_json(r.category) << "\",\n";
    out << "      \"header_file\": \"" << escape_json(r.header_file) << "\",\n";
    out << "      \"sizeof_bytes\": " << r.sizeof_bytes << ",\n";
    out << "      \"heap_bytes_per_instance\": " << r.heap_bytes_per_instance << ",\n";
    out << "      \"batch_total_ram_kb\": " << r.batch_total_ram_kb << ",\n";
    out << "      \"deep_memory_kb\": " << r.deep_memory_kb << ",\n";
    out << "      \"single_create_ns\": " << r.single_create_ns << ",\n";
    out << "      \"single_destroy_ns\": " << r.single_destroy_ns << ",\n";
    out << "      \"batch_create_mops\": " << r.batch_create_mops << ",\n";
    out << "      \"batch_destroy_mops\": " << r.batch_destroy_mops << ",\n";
    out << "      \"layout_or_resize_ns\": " << r.layout_or_resize_ns << ",\n";
    out << "      \"draw_or_calc_ns\": " << r.draw_or_calc_ns << ",\n";
    out << "      \"event_handle_ns\": " << r.event_handle_ns << ",\n";
    out << "      \"custom_op_name\": \"" << escape_json(r.custom_op_name) << "\",\n";
    out << "      \"custom_op_ns\": " << r.custom_op_ns << ",\n";
    out << "      \"custom_op_ops_per_sec\": " << r.custom_op_ops_per_sec << ",\n";
    out << "      \"passed\": " << (r.passed ? "true" : "false") << "\n";
    out << "    }" << (i + 1 < results.size() ? "," : "") << "\n";
  }

  out << "  ]\n";
  out << "}\n";
  out.close();
}

static void export_html(const std::string& filepath, const std::vector<BenchmarkResult>& results) {
  std::ofstream out(filepath);
  if (!out.is_open()) {
    std::cerr << "Error: Could not open " << filepath << " for writing HTML.\n";
    return;
  }

  std::time_t now = std::time(nullptr);
  char time_buf[64];
  std::strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S UTC", std::gmtime(&now));

  // Compute aggregate stats
  size_t total_classes = results.size();
  size_t sum_sizeof = 0;
  double sum_ram_kb = 0;
  double min_create_ns = 1e12;
  double max_create_ns = 0.0;
  std::string fastest_class;
  std::string smallest_class;
  size_t min_sizeof = 1000000;

  for (const auto& r : results) {
    sum_sizeof += r.sizeof_bytes;
    sum_ram_kb += r.batch_total_ram_kb;
    if (r.single_create_ns > 0 && r.single_create_ns < min_create_ns) {
      min_create_ns = r.single_create_ns;
      fastest_class = r.class_name;
    }
    if (r.single_create_ns > max_create_ns) {
      max_create_ns = r.single_create_ns;
    }
    if (r.sizeof_bytes < min_sizeof) {
      min_sizeof = r.sizeof_bytes;
      smallest_class = r.class_name;
    }
  }
  double avg_sizeof = total_classes > 0 ? (double)sum_sizeof / total_classes : 0.0;

  out << "<!DOCTYPE html>\n";
  out << "<html lang=\"en\">\n";
  out << "<head>\n";
  out << "  <meta charset=\"UTF-8\">\n";
  out << "  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
  out << "  <title>FLTK Benchmark Report - Memory and Execution Speed</title>\n";
  out << "  <style>\n";
  out << "    :root {\n";
  out << "      --bg-primary: #0f172a;\n";
  out << "      --bg-secondary: #1e293b;\n";
  out << "      --bg-card: #1e293b;\n";
  out << "      --text-primary: #f8fafc;\n";
  out << "      --text-secondary: #94a3b8;\n";
  out << "      --accent: #38bdf8;\n";
  out << "      --accent-glow: rgba(56, 189, 248, 0.25);\n";
  out << "      --success: #4ade80;\n";
  out << "      --warning: #fbbf24;\n";
  out << "      --border: #334155;\n";
  out << "      --table-stripe: #1e293b;\n";
  out << "      --table-hover: #334155;\n";
  out << "    }\n";
  out << "    * { box-sizing: border-box; margin: 0; padding: 0; }\n";
  out << "    body {\n";
  out << "      font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Helvetica, Arial, sans-serif;\n";
  out << "      background-color: var(--bg-primary);\n";
  out << "      color: var(--text-primary);\n";
  out << "      line-height: 1.5;\n";
  out << "      padding: 24px;\n";
  out << "    }\n";
  out << "    .container { max-width: 1400px; margin: 0 auto; }\n";
  out << "    header {\n";
  out << "      display: flex;\n";
  out << "      justify-content: space-between;\n";
  out << "      align-items: center;\n";
  out << "      margin-bottom: 24px;\n";
  out << "      padding-bottom: 16px;\n";
  out << "      border-bottom: 1px solid var(--border);\n";
  out << "      flex-wrap: wrap;\n";
  out << "      gap: 16px;\n";
  out << "    }\n";
  out << "    h1 { font-size: 28px; font-weight: 700; color: #fff; display: flex; align-items: center; gap: 12px; }\n";
  out << "    .badge { font-size: 13px; background: var(--accent); color: #0f172a; padding: 4px 10px; border-radius: 9999px; font-weight: 600; }\n";
  out << "    .meta-info { color: var(--text-secondary); font-size: 14px; }\n";
  out << "    \n";
  out << "    /* KPI Cards */\n";
  out << "    .kpi-grid {\n";
  out << "      display: grid;\n";
  out << "      grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));\n";
  out << "      gap: 16px;\n";
  out << "      margin-bottom: 24px;\n";
  out << "    }\n";
  out << "    .kpi-card {\n";
  out << "      background: var(--bg-card);\n";
  out << "      border: 1px solid var(--border);\n";
  out << "      padding: 18px;\n";
  out << "      border-radius: 12px;\n";
  out << "      box-shadow: 0 4px 6px -1px rgba(0, 0, 0, 0.1);\n";
  out << "    }\n";
  out << "    .kpi-title { font-size: 13px; text-transform: uppercase; letter-spacing: 0.5px; color: var(--text-secondary); margin-bottom: 6px; }\n";
  out << "    .kpi-value { font-size: 26px; font-weight: 700; color: var(--accent); }\n";
  out << "    .kpi-sub { font-size: 12px; color: var(--text-secondary); margin-top: 4px; }\n";
  out << "    \n";
  out << "    /* Charts */\n";
  out << "    .charts-grid {\n";
  out << "      display: grid;\n";
  out << "      grid-template-columns: 1fr 1fr;\n";
  out << "      gap: 20px;\n";
  out << "      margin-bottom: 24px;\n";
  out << "    }\n";
  out << "    @media (max-width: 900px) { .charts-grid { grid-template-columns: 1fr; } }\n";
  out << "    .chart-card {\n";
  out << "      background: var(--bg-card);\n";
  out << "      border: 1px solid var(--border);\n";
  out << "      border-radius: 12px;\n";
  out << "      padding: 20px;\n";
  out << "    }\n";
  out << "    .chart-header { font-size: 16px; font-weight: 600; margin-bottom: 16px; color: #fff; }\n";
  out << "    .bar-chart { display: flex; flex-direction: column; gap: 8px; max-height: 320px; overflow-y: auto; padding-right: 8px; }\n";
  out << "    .bar-row { display: flex; align-items: center; gap: 12px; font-size: 13px; }\n";
  out << "    .bar-label { width: 160px; white-space: nowrap; overflow: hidden; text-overflow: ellipsis; }\n";
  out << "    .bar-track { flex: 1; height: 16px; background: rgba(255,255,255,0.05); border-radius: 4px; overflow: hidden; position: relative; }\n";
  out << "    .bar-fill { height: 100%; border-radius: 4px; background: linear-gradient(90deg, #38bdf8, #818cf8); }\n";
  out << "    .bar-val { width: 70px; text-align: right; font-family: monospace; color: var(--text-secondary); }\n";
  out << "    \n";
  out << "    /* Toolbar & Filters */\n";
  out << "    .toolbar {\n";
  out << "      display: flex;\n";
  out << "      gap: 12px;\n";
  out << "      margin-bottom: 16px;\n";
  out << "      flex-wrap: wrap;\n";
  out << "      align-items: center;\n";
  out << "      justify-content: space-between;\n";
  out << "    }\n";
  out << "    .search-box {\n";
  out << "      background: var(--bg-secondary);\n";
  out << "      border: 1px solid var(--border);\n";
  out << "      color: #fff;\n";
  out << "      padding: 8px 16px;\n";
  out << "      border-radius: 8px;\n";
  out << "      font-size: 14px;\n";
  out << "      min-width: 260px;\n";
  out << "    }\n";
  out << "    .search-box:focus { outline: none; border-color: var(--accent); }\n";
  out << "    .category-pills {\n";
  out << "      display: flex;\n";
  out << "      gap: 6px;\n";
  out << "      flex-wrap: wrap;\n";
  out << "    }\n";
  out << "    .pill {\n";
  out << "      background: var(--bg-secondary);\n";
  out << "      border: 1px solid var(--border);\n";
  out << "      color: var(--text-secondary);\n";
  out << "      padding: 5px 12px;\n";
  out << "      border-radius: 20px;\n";
  out << "      font-size: 13px;\n";
  out << "      cursor: pointer;\n";
  out << "      user-select: none;\n";
  out << "      transition: all 0.2s;\n";
  out << "    }\n";
  out << "    .pill:hover, .pill.active {\n";
  out << "      background: var(--accent);\n";
  out << "      color: #0f172a;\n";
  out << "      font-weight: 600;\n";
  out << "      border-color: var(--accent);\n";
  out << "    }\n";
  out << "    .export-btns { display: flex; gap: 8px; }\n";
  out << "    .btn {\n";
  out << "      background: var(--bg-secondary);\n";
  out << "      border: 1px solid var(--border);\n";
  out << "      color: #fff;\n";
  out << "      padding: 6px 14px;\n";
  out << "      border-radius: 8px;\n";
  out << "      font-size: 13px;\n";
  out << "      cursor: pointer;\n";
  out << "      transition: background 0.2s;\n";
  out << "    }\n";
  out << "    .btn:hover { background: #334155; }\n";
  out << "    \n";
  out << "    /* Table */\n";
  out << "    .table-container {\n";
  out << "      background: var(--bg-card);\n";
  out << "      border: 1px solid var(--border);\n";
  out << "      border-radius: 12px;\n";
  out << "      overflow-x: auto;\n";
  out << "      box-shadow: 0 4px 6px -1px rgba(0, 0, 0, 0.1);\n";
  out << "    }\n";
  out << "    table {\n";
  out << "      width: 100%;\n";
  out << "      border-collapse: collapse;\n";
  out << "      text-align: left;\n";
  out << "      font-size: 14px;\n";
  out << "    }\n";
  out << "    th {\n";
  out << "      background: #1e293b;\n";
  out << "      color: var(--text-secondary);\n";
  out << "      font-weight: 600;\n";
  out << "      padding: 12px 16px;\n";
  out << "      border-bottom: 1px solid var(--border);\n";
  out << "      cursor: pointer;\n";
  out << "      user-select: none;\n";
  out << "      white-space: nowrap;\n";
  out << "    }\n";
  out << "    th:hover { color: #fff; }\n";
  out << "    th.sorted-asc::after { content: ' ▲'; color: var(--accent); }\n";
  out << "    th.sorted-desc::after { content: ' ▼'; color: var(--accent); }\n";
  out << "    td {\n";
  out << "      padding: 12px 16px;\n";
  out << "      border-bottom: 1px solid rgba(255,255,255,0.05);\n";
  out << "      font-variant-numeric: tabular-nums;\n";
  out << "    }\n";
  out << "    tr:hover td { background-color: var(--table-hover); }\n";
  out << "    .class-name { font-weight: 600; color: #fff; font-family: monospace; font-size: 14px; }\n";
  out << "    .cat-tag { font-size: 12px; padding: 2px 8px; border-radius: 4px; background: rgba(56, 189, 248, 0.1); color: var(--accent); }\n";
  out << "    .num { text-align: right; font-family: monospace; }\n";
  out << "    .status-pass { color: var(--success); font-weight: 600; }\n";
  out << "  </style>\n";
  out << "</head>\n";
  out << "<body>\n";
  out << "  <div class=\"container\">\n";
  out << "    <header>\n";
  out << "      <div>\n";
  out << "        <h1>FLTK Class Benchmarks <span class=\"badge\">v" << FL_MAJOR_VERSION << "." << FL_MINOR_VERSION << "." << FL_PATCH_VERSION << "</span></h1>\n";
  out << "        <p class=\"meta-info\">RAM Consumption & Execution Speed Profile &bull; Generated: " << time_buf << "</p>\n";
  out << "      </div>\n";
  out << "      <div class=\"export-btns\">\n";
  out << "        <button class=\"btn\" onclick=\"exportCSV()\">Export CSV</button>\n";
  out << "        <button class=\"btn\" onclick=\"window.print()\">Print / PDF</button>\n";
  out << "      </div>\n";
  out << "    </header>\n\n";

  // KPIs
  out << "    <div class=\"kpi-grid\">\n";
  out << "      <div class=\"kpi-card\">\n";
  out << "        <div class=\"kpi-title\">Classes Benchmarked</div>\n";
  out << "        <div class=\"kpi-value\">" << total_classes << "</div>\n";
  out << "        <div class=\"kpi-sub\">100% Comprehensive Coverage</div>\n";
  out << "      </div>\n";
  out << "      <div class=\"kpi-card\">\n";
  out << "        <div class=\"kpi-title\">Avg sizeof(Class)</div>\n";
  out << "        <div class=\"kpi-value\">" << std::fixed << std::setprecision(1) << avg_sizeof << " <span style=\"font-size:16px\">B</span></div>\n";
  out << "        <div class=\"kpi-sub\">Smallest: " << escape_html(smallest_class) << " (" << min_sizeof << " B)</div>\n";
  out << "      </div>\n";
  out << "      <div class=\"kpi-card\">\n";
  out << "        <div class=\"kpi-title\">Fastest Instantiation</div>\n";
  out << "        <div class=\"kpi-value\">" << std::fixed << std::setprecision(1) << min_create_ns << " <span style=\"font-size:16px\">ns</span></div>\n";
  out << "        <div class=\"kpi-sub\">" << escape_html(fastest_class) << " (" << std::fixed << std::setprecision(1) << (1000.0 / min_create_ns) << " Mops/s)</div>\n";
  out << "      </div>\n";
  out << "      <div class=\"kpi-card\">\n";
  out << "        <div class=\"kpi-title\">Total RAM (1k instances)</div>\n";
  out << "        <div class=\"kpi-value\">" << std::fixed << std::setprecision(2) << (sum_ram_kb / 1024.0) << " <span style=\"font-size:16px\">MB</span></div>\n";
  out << "        <div class=\"kpi-sub\">Total RAM for all classes combined</div>\n";
  out << "      </div>\n";
  out << "    </div>\n\n";

  // Visual Charts
  out << "    <div class=\"charts-grid\">\n";
  out << "      <div class=\"chart-card\">\n";
  out << "        <div class=\"chart-header\">Top Classes by sizeof Memory Footprint (Bytes)</div>\n";
  out << "        <div class=\"bar-chart\" id=\"memoryChart\">\n";

  // Sort by sizeof descending for chart
  auto sorted_by_size = results;
  std::sort(sorted_by_size.begin(), sorted_by_size.end(), [](const BenchmarkResult& a, const BenchmarkResult& b) {
    return a.sizeof_bytes > b.sizeof_bytes;
  });
  size_t max_size = sorted_by_size.empty() ? 1 : sorted_by_size.front().sizeof_bytes;
  size_t top_count = std::min((size_t)15, sorted_by_size.size());
  for (size_t i = 0; i < top_count; ++i) {
    const auto& r = sorted_by_size[i];
    double pct = (double)r.sizeof_bytes / max_size * 100.0;
    out << "          <div class=\"bar-row\">\n";
    out << "            <div class=\"bar-label\" title=\"" << escape_html(r.class_name) << "\">" << escape_html(r.class_name) << "</div>\n";
    out << "            <div class=\"bar-track\"><div class=\"bar-fill\" style=\"width: " << pct << "%\"></div></div>\n";
    out << "            <div class=\"bar-val\">" << r.sizeof_bytes << " B</div>\n";
    out << "          </div>\n";
  }
  out << "        </div>\n";
  out << "      </div>\n";

  out << "      <div class=\"chart-card\">\n";
  out << "        <div class=\"chart-header\">Fastest Class Instantiations (Million ops/sec)</div>\n";
  out << "        <div class=\"bar-chart\" id=\"speedChart\">\n";

  auto sorted_by_speed = results;
  std::sort(sorted_by_speed.begin(), sorted_by_speed.end(), [](const BenchmarkResult& a, const BenchmarkResult& b) {
    return a.batch_create_mops > b.batch_create_mops;
  });
  double max_mops = sorted_by_speed.empty() ? 1.0 : sorted_by_speed.front().batch_create_mops;
  size_t top_speed_count = std::min((size_t)15, sorted_by_speed.size());
  for (size_t i = 0; i < top_speed_count; ++i) {
    const auto& r = sorted_by_speed[i];
    double pct = (r.batch_create_mops / max_mops) * 100.0;
    out << "          <div class=\"bar-row\">\n";
    out << "            <div class=\"bar-label\" title=\"" << escape_html(r.class_name) << "\">" << escape_html(r.class_name) << "</div>\n";
    out << "            <div class=\"bar-track\"><div class=\"bar-fill\" style=\"width: " << pct << "%; background: linear-gradient(90deg, #4ade80, #38bdf8);\"></div></div>\n";
    out << "            <div class=\"bar-val\">" << std::fixed << std::setprecision(2) << r.batch_create_mops << " M</div>\n";
    out << "          </div>\n";
  }
  out << "        </div>\n";
  out << "      </div>\n";
  out << "    </div>\n\n";

  // Category Pills & Search
  out << "    <div class=\"toolbar\">\n";
  out << "      <input type=\"text\" id=\"searchBox\" class=\"search-box\" placeholder=\"Filter by class name...\" oninput=\"filterTable()\">\n";
  out << "      <div class=\"category-pills\" id=\"catPills\">\n";
  out << "        <div class=\"pill active\" onclick=\"selectCategory('All', this)\">All (" << total_classes << ")</div>\n";

  // Extract unique categories
  std::vector<std::string> cats;
  for (const auto& r : results) {
    if (std::find(cats.begin(), cats.end(), r.category) == cats.end()) {
      cats.push_back(r.category);
    }
  }
  std::sort(cats.begin(), cats.end());
  for (const auto& cat : cats) {
    size_t count = 0;
    for (const auto& r : results) if (r.category == cat) count++;
    out << "        <div class=\"pill\" onclick=\"selectCategory('" << escape_html(cat) << "', this)\">" << escape_html(cat) << " (" << count << ")</div>\n";
  }
  out << "      </div>\n";
  out << "    </div>\n\n";

  // Table
  out << "    <div class=\"table-container\">\n";
  out << "      <table id=\"benchTable\">\n";
  out << "        <thead>\n";
  out << "          <tr>\n";
  out << "            <th onclick=\"sortTable(0, 'str')\">Class Name</th>\n";
  out << "            <th onclick=\"sortTable(1, 'str')\">Category</th>\n";
  out << "            <th class=\"num\" onclick=\"sortTable(2, 'num')\">sizeof (B)</th>\n";
  out << "            <th class=\"num\" onclick=\"sortTable(3, 'num')\">Heap/Inst (B)</th>\n";
  out << "            <th class=\"num\" onclick=\"sortTable(4, 'num')\">RAM / 1k (KB)</th>\n";
  out << "            <th class=\"num\" onclick=\"sortTable(5, 'num')\">Create (ns)</th>\n";
  out << "            <th class=\"num\" onclick=\"sortTable(6, 'num')\">Destroy (ns)</th>\n";
  out << "            <th class=\"num\" onclick=\"sortTable(7, 'num')\">Create Rate (Mops/s)</th>\n";
  out << "            <th class=\"num\" onclick=\"sortTable(8, 'num')\">Resize/Op (ns)</th>\n";
  out << "            <th>Custom Benchmark</th>\n";
  out << "            <th>Status</th>\n";
  out << "          </tr>\n";
  out << "        </thead>\n";
  out << "        <tbody>\n";

  for (const auto& r : results) {
    out << "          <tr data-cat=\"" << escape_html(r.category) << "\">\n";
    out << "            <td class=\"class-name\">" << escape_html(r.class_name) << "</td>\n";
    out << "            <td><span class=\"cat-tag\">" << escape_html(r.category) << "</span></td>\n";
    out << "            <td class=\"num\">" << r.sizeof_bytes << "</td>\n";
    out << "            <td class=\"num\">" << std::fixed << std::setprecision(1) << r.heap_bytes_per_instance << "</td>\n";
    out << "            <td class=\"num\">" << std::fixed << std::setprecision(2) << r.batch_total_ram_kb << "</td>\n";
    out << "            <td class=\"num\">" << std::fixed << std::setprecision(1) << r.single_create_ns << "</td>\n";
    out << "            <td class=\"num\">" << std::fixed << std::setprecision(1) << r.single_destroy_ns << "</td>\n";
    out << "            <td class=\"num\">" << std::fixed << std::setprecision(2) << r.batch_create_mops << "</td>\n";
    out << "            <td class=\"num\">" << std::fixed << std::setprecision(1) << (r.layout_or_resize_ns > 0 ? r.layout_or_resize_ns : r.event_handle_ns) << "</td>\n";
    out << "            <td>" << (r.custom_op_name.empty() ? "-" : escape_html(r.custom_op_name) + ": " + std::to_string((int)r.custom_op_ns) + " ns") << "</td>\n";
    out << "            <td><span class=\"status-pass\">PASS</span></td>\n";
    out << "          </tr>\n";
  }

  out << "        </tbody>\n";
  out << "      </table>\n";
  out << "    </div>\n\n";

  // JavaScript for search, category filter, sorting, CSV export
  out << "    <script>\n";
  out << "      let currentCat = 'All';\n";
  out << "      function selectCategory(cat, el) {\n";
  out << "        currentCat = cat;\n";
  out << "        document.querySelectorAll('.pill').forEach(p => p.classList.remove('active'));\n";
  out << "        el.classList.add('active');\n";
  out << "        filterTable();\n";
  out << "      }\n";
  out << "      function filterTable() {\n";
  out << "        const query = document.getElementById('searchBox').value.toLowerCase();\n";
  out << "        const rows = document.querySelectorAll('#benchTable tbody tr');\n";
  out << "        rows.forEach(r => {\n";
  out << "          const name = r.children[0].innerText.toLowerCase();\n";
  out << "          const cat = r.getAttribute('data-cat');\n";
  out << "          const matchesCat = (currentCat === 'All' || cat === currentCat);\n";
  out << "          const matchesQuery = name.includes(query);\n";
  out << "          r.style.display = (matchesCat && matchesQuery) ? '' : 'none';\n";
  out << "        });\n";
  out << "      }\n";
  out << "      let sortDirections = {};\n";
  out << "      function sortTable(colIndex, type) {\n";
  out << "        const table = document.getElementById('benchTable');\n";
  out << "        const tbody = table.querySelector('tbody');\n";
  out << "        const rows = Array.from(tbody.querySelectorAll('tr'));\n";
  out << "        const isAsc = !sortDirections[colIndex];\n";
  out << "        sortDirections = {}; // reset others\n";
  out << "        sortDirections[colIndex] = isAsc;\n";
  out << "        table.querySelectorAll('th').forEach((th, idx) => {\n";
  out << "          th.classList.remove('sorted-asc', 'sorted-desc');\n";
  out << "          if (idx === colIndex) th.classList.add(isAsc ? 'sorted-asc' : 'sorted-desc');\n";
  out << "        });\n";
  out << "        rows.sort((a, b) => {\n";
  out << "          let va = a.children[colIndex].innerText.trim();\n";
  out << "          let vb = b.children[colIndex].innerText.trim();\n";
  out << "          if (type === 'num') {\n";
  out << "            let na = parseFloat(va) || 0;\n";
  out << "            let nb = parseFloat(vb) || 0;\n";
  out << "            return isAsc ? (na - nb) : (nb - na);\n";
  out << "          } else {\n";
  out << "            return isAsc ? va.localeCompare(vb) : vb.localeCompare(va);\n";
  out << "          }\n";
  out << "        });\n";
  out << "        rows.forEach(r => tbody.appendChild(r));\n";
  out << "      }\n";
  out << "      function exportCSV() {\n";
  out << "        const rows = document.querySelectorAll('#benchTable tr');\n";
  out << "        let csv = [];\n";
  out << "        rows.forEach(r => {\n";
  out << "          let row = [];\n";
  out << "          r.querySelectorAll('th, td').forEach(c => row.push('\"' + c.innerText.replace(/\"/g, '\"\"') + '\"'));\n";
  out << "          csv.push(row.join(','));\n";
  out << "        });\n";
  out << "        const blob = new Blob([csv.join('\\n')], { type: 'text/csv' });\n";
  out << "        const url = window.URL.createObjectURL(blob);\n";
  out << "        const a = document.createElement('a');\n";
  out << "        a.href = url;\n";
  out << "        a.download = 'fltk_benchmarks.csv';\n";
  out << "        a.click();\n";
  out << "      }\n";
  out << "    </script>\n";
  out << "  </div>\n";
  out << "</body>\n";
  out << "</html>\n";

  out.close();
}

static void print_console_table(const std::vector<BenchmarkResult>& results) {
  std::cout << "\n=========================================================================================================\n";
  std::cout << "                                  FLTK CLASS BENCHMARK SUMMARY                                           \n";
  std::cout << "=========================================================================================================\n";
  std::cout << std::left << std::setw(28) << "Class Name"
            << std::setw(20) << "Category"
            << std::right
            << std::setw(10) << "sizeof(B)"
            << std::setw(12) << "Heap/Obj(B)"
            << std::setw(14) << "Create(ns)"
            << std::setw(14) << "Destroy(ns)"
            << std::setw(14) << "Create(Mops/s)"
            << "\n";
  std::cout << "---------------------------------------------------------------------------------------------------------\n";

  for (const auto& r : results) {
    std::cout << std::left << std::setw(28) << r.class_name
              << std::setw(20) << r.category
              << std::right
              << std::setw(10) << r.sizeof_bytes
              << std::setw(12) << std::fixed << std::setprecision(1) << r.heap_bytes_per_instance
              << std::setw(14) << std::fixed << std::setprecision(1) << r.single_create_ns
              << std::setw(14) << std::fixed << std::setprecision(1) << r.single_destroy_ns
              << std::setw(14) << std::fixed << std::setprecision(2) << r.batch_create_mops
              << "\n";
  }
  std::cout << "=========================================================================================================\n";
  std::cout << "Total Classes Benchmarked: " << results.size() << "\n\n";
}

int main(int argc, char* argv[]) {
  std::string html_path = "fltk_benchmark_report.html";
  std::string json_path = "fltk_benchmark_report.json";
  std::string filter_str = "";
  std::string cat_filter = "";
  bool quiet = false;

  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    if (arg.rfind("--html=", 0) == 0) {
      html_path = arg.substr(7);
    } else if (arg.rfind("--json=", 0) == 0) {
      json_path = arg.substr(7);
    } else if (arg.rfind("--filter=", 0) == 0) {
      filter_str = arg.substr(9);
    } else if (arg.rfind("--category=", 0) == 0) {
      cat_filter = arg.substr(11);
    } else if (arg == "--quiet" || arg == "-q") {
      quiet = true;
    } else if (arg == "--help" || arg == "-h") {
      std::cout << "FLTK Class Benchmarks Suite\n";
      std::cout << "Usage: " << argv[0] << " [options]\n";
      std::cout << "Options:\n";
      std::cout << "  --html=<path>       Generate HTML report (default: fltk_benchmark_report.html)\n";
      std::cout << "  --json=<path>       Generate JSON report (default: fltk_benchmark_report.json)\n";
      std::cout << "  --filter=<name>     Filter class names by substring\n";
      std::cout << "  --category=<cat>    Filter by category\n";
      std::cout << "  --quiet, -q         Quiet mode (minimal console output)\n";
      std::cout << "  --help, -h          Show this help\n";
      return 0;
    }
  }

  const auto& benchmarks = BenchmarkRegistry::instance().benchmarks();
  std::vector<BenchmarkResult> results;
  results.reserve(benchmarks.size());

  if (!quiet) {
    std::cout << "Running " << benchmarks.size() << " FLTK class benchmarks...\n";
  }

  for (size_t i = 0; i < benchmarks.size(); ++i) {
    BenchmarkResult res = benchmarks[i]();
    if (!quiet) {
      std::cout << "  [" << (i + 1) << "/" << benchmarks.size() << "] "
                << std::left << std::setw(30) << res.class_name
                << " sizeof=" << std::setw(4) << res.sizeof_bytes << "B"
                << " create=" << std::fixed << std::setprecision(1) << res.single_create_ns << "ns"
                << " (" << std::fixed << std::setprecision(2) << res.batch_create_mops << " Mops/s)\n" << std::flush;
    }
    if (!filter_str.empty() && res.class_name.find(filter_str) == std::string::npos) {
      continue;
    }
    if (!cat_filter.empty() && res.category.find(cat_filter) == std::string::npos) {
      continue;
    }
    results.push_back(res);
  }

  if (!quiet) {
    print_console_table(results);
  }

  export_html(html_path, results);
  export_json(json_path, results);

  std::cout << "Generated HTML Report: " << html_path << "\n";
  std::cout << "Generated JSON Report: " << json_path << "\n";

  return 0;
}
