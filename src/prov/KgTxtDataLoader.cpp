#include "KgTxtDataLoader.h"
#include <fstream>
#include <string>
#include <regex>
#include <assert.h>
#include "base/KuStrUtil.h"


KgTxtDataLoader::KgTxtDataLoader()
{
    illegalMode_ = k_illegal_ignore;
    emptyMode_ = k_empty_skip;
    //rowMajor_ = false;
    rexpNA_ = "na|nan|n/a|-";
    rexpDelim_ = "\\s+";
    rexpComment_ = "^\\s*#";
}


bool KgTxtDataLoader::load(const char* path, std::vector<std::vector<double>>& mat) const
{
    mat.clear();

    std::ifstream ifs(path);
    std::string line;
    while (std::getline(ifs, line)) {
       
        // ÏÈ¼ì²âcomment
        if (!rexpComment_.empty()) {
            const std::regex re(rexpComment_);
            std::smatch sm;
            if (std::regex_search(line, re))
                continue; // Ìø¹ý×¢ÊÍÐÐ
        }

        auto tokens = KuStrUtil::splitRegex(line, rexpDelim_, emptyMode_ == k_empty_skip);
        if (tokens.empty())
            continue; // always skip empty line

        std::vector<double> row;
        row.reserve(tokens.size());
        for (auto& tok : tokens) {

            // ¿Õ×Ö·û´®¼ì²â
            if (tok.empty()) {
                if (emptyMode_ != k_empty_skip)
                    row.push_back(nullValue());
                continue;
            }

            // NA×Ö·û´®¼ì²â
            const std::regex re(rexpNA_);
            std::smatch sm;
            if (std::regex_match(line, re)) {
                row.push_back(std::numeric_limits<double>::quiet_NaN());
                continue;
            }

            // ³¢ÊÔ½âÎöÊý×Ö
            double val;
            if (!KuStrUtil::toValue(tok.c_str(), val)) {
                switch (illegalMode_) {
                case k_illegal_fail: return false;
                case k_illegal_ignore: continue;

                case k_illegal_as_nan:
                    val = std::numeric_limits<double>::quiet_NaN();
                    break;
                case k_illegal_as_zero:
                    val = 0;
                    break;
                case k_illegal_as_empty:
                    val = nullValue();
                    break;
                }
            }

            row.push_back(val);
        }

        if (!row.empty())
            mat.push_back(row);
    }

    return true;
}


double KgTxtDataLoader::nullValue() const
{
    return emptyMode_ == k_empty_as_zero ? 0 : std::numeric_limits<double>::quiet_NaN();
}

