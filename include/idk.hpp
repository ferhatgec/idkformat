// MIT License
//
// Copyright (c) 2022 Ferhat Geçdoğan All Rights Reserved.
// Distributed under the terms of the MIT License.
//
// idkformat (i don't know format) - header only world's simplest & stupid- runtime-ti formatter
// ---------------------------------------------------------------------------------------------
// * there's no type wrapper yet. if anyone want to implement see first TODO
//                                                                      where is good to start.
// * there's runtime dynamic casting for supported types, so unimplemented types are need
// a support for un/directly casting to STL string.
//
// some macros:
// ------------
// * $ = gives last index argument list to formatter
//  * idk::format("{$}", 13, "gech") -> gives "gech"
// * D = duplicates specific argument by given number of times with optional delimiter ({D:argument:n}, {D:argument:n:'character'}
//                                                                                                      note: within '..' block like 'e')
//  * idk::format("g{D:0:2}ch", 'e') -> gives "geech"
//  * idk::format("g{D:0:2:'!'}ch", 'e') -> gives "ge!e!ch"
// * A = absolute macro for given integer
//  * idk::format("{A}", -100) -> gives "100"
//  * idk::format("{A:1}", 13, -42) -> gives "42"
// * U, L = upper/lowercase macro for given string
//  * idk::format("{U}", "gech") -> gives "GECH"
//  * idk::format("{L}", "GECH") -> gives "gech"
//  * idk::format("{U} {L}", "mr.", "Gech") -> gives "MR. gech"
// * B = converts decimal to binary by given integer with leading zeros.
//  * idk::format("{B}", 13) -> gives 000..."1101"
// * Z = converts decimal to binary by given integer with strip off leading zeros.
//  * idk::format("{Z}", 13) -> gives "1101"
//  * idk::format("{Z} = {Z}", 13, "13") -> gives "1101 = 1101"
//  * idk::format("{Z} = {Z: $}", "13", "13") -> gives "1101 = 1101"
//
// github.com/ferhatgec/idkformat


#include <any>
#include <algorithm>
#include <vector>
#include <string>
#include <iostream>
#include <bitset>
#include <typeinfo>

namespace helper {
    static inline unsigned abs(long long int x) noexcept {
        return (x < 0) ? -x : x;
    }

    static bool is_number(const std::string& s) {
        if(!s.empty() && s.front() == '-')  return !s.empty() && std::find_if(s.begin() + 1,
                                                                              s.end(), [](unsigned char c) { return !std::isdigit(c); }) == s.end();

        return !s.empty() && std::find_if(s.begin(),
                                          s.end(), [](unsigned char c) { return !std::isdigit(c); }) == s.end();
    }

    static inline void ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
            return !std::isspace(ch);
        }));
    }

    static inline void rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
            return !std::isspace(ch);
        }).base(), s.end());
    }

    static inline void trim(std::string &s) {
        helper::ltrim(s);
        helper::rtrim(s);
    }

    std::vector<std::string> split(const std::string& str, char delim, std::size_t args = 0) {
        std::vector<std::string> strings;
        std::size_t start;
        std::size_t end = 0;
        while ((start = str.find_first_not_of(delim, end)) != std::string::npos) {
            end = str.find(delim, start);
            strings.push_back(str.substr(start, end - start));
        }

        for(auto& val: strings) { if(val.find('$') != val.npos) { val = std::to_string(args); }}
        return strings;
    }
}

namespace idk {
    template<typename Type>
    Type __any_cast__(std::any arg) {
        try {
            return std::any_cast<Type>(arg);
        } catch(std::bad_alloc const& err) {
            return static_cast<Type>(0);
        }
    }

    std::string __to_string__(std::any& arg) noexcept {
        const auto hash = arg.type().hash_code();

        if(hash == typeid(short).hash_code()) return std::to_string(idk::__any_cast__<short>(arg));
        else if(hash == typeid(unsigned short).hash_code()) return std::to_string(idk::__any_cast__<unsigned short>(arg));
        else if(hash == typeid(bool).hash_code()) {
            auto data = idk::__any_cast__<bool>(arg);
            return std::string(data == true ? "true" : "false");
        } else if(hash == typeid(int).hash_code()) return std::to_string(idk::__any_cast__<int>(arg));
        else if(hash == typeid(unsigned).hash_code()) return std::to_string(idk::__any_cast__<unsigned>(arg));
        else if(hash == typeid(long).hash_code()) return std::to_string(idk::__any_cast__<long>(arg));
        else if(hash == typeid(unsigned long).hash_code()) return std::to_string(idk::__any_cast__<unsigned long>(arg));
        else if(hash == typeid(long long).hash_code()) return std::to_string(idk::__any_cast__<long long>(arg));
        else if(hash == typeid(unsigned long long).hash_code()) return std::to_string(idk::__any_cast__<unsigned long long>(arg));
        else if(hash == typeid(char).hash_code()) return std::string(1, idk::__any_cast__<char>(arg));
        else if(hash == typeid(const char*).hash_code()) return std::string(idk::__any_cast__<const char*>(arg));
        else if(hash == typeid(char*).hash_code()) return std::string(idk::__any_cast__<char*>(arg));
        else if(hash == typeid(std::string).hash_code()) return idk::__any_cast__<std::string>(arg);
        else { return ""; } // TODO: wrapper for unimplemented types (like classes etc.)
    }


    template<typename... Args>
    std::string format(const std::string data, Args... args) noexcept {
        std::vector<std::any> vals { args... };
        unsigned count = 0;
        const std::size_t __len__ = vals.size() - 1;
        std::string replacement = "";

        for(std::size_t i = 0; i < data.size(); ++i) {
            switch(data[i]) {
                case '{': {
                    if(i + 1 < data.size()) {
                        if(data[i + 1] == '{') {
                            replacement.push_back('{');
                            ++i;
                        } else if(data[i + 1] == '}') {
                            replacement.append(idk::__to_string__(vals[count++]));
                            ++i;
                        } else {
                            std::string attr;

                            for(++i; data[i] != '}' && i < data.size(); ++i)
                                attr.push_back(data[i]);

                            if(helper::is_number(attr)) {
                                unsigned __attr__ = std::stoi(attr);

                                if(__attr__ < vals.size()) {
                                    replacement.append(idk::__to_string__(vals[std::stoi(attr)]));
                                    count = __attr__;
                                }
                            } else if(attr.front() == '$') {
                                if(!vals.empty()) {
                                    replacement.append(idk::__to_string__(vals.back()));
                                    count = vals.size() - 1;
                                }
                            } else if(attr.front() == 'D') {
                                if(attr.length() >= 2) {
                                    // {D:id:n: ' '}
                                    auto val = helper::split(attr, ':', __len__);
                                    helper::trim(val[0]); helper::trim(val[1]); helper::trim(val[2]);

                                    if(val.size() >= 3 && val[0] == "D") {
                                        if(helper::is_number(val[1]) && helper::is_number(val[2])) {
                                            const unsigned __val__ = std::stoi(val[1]),
                                            __n__   = std::stoi(val[2]);

                                            if(__val__ < vals.size()) {
                                                const std::string __alloc__ = idk::__to_string__(vals[__val__]);
                                                std::string replace_data = "";

                                                if(val.size() > 3) {
                                                    helper::trim(val[3]);

                                                    if(val[3].front() == '\'') {
                                                        for(std::size_t __index__ = 1; __index__ < val[3].length() - 1; ++__index__)
                                                            replace_data.push_back(val[3][__index__]);
                                                    }
                                                }

                                                if(replace_data.empty()) for(unsigned i = 0; i < __n__; ++i)
                                                    replacement.append(__alloc__);
                                                else for(unsigned i = 0; i < __n__; ++i)
                                                    replacement.append(__alloc__ + replace_data);
                                            }
                                        }
                                    }
                                }
                            } else if(attr.front() == 'A') {
                                auto val = helper::split(attr, ':', __len__);

                                if(val.size() >= 1) {
                                    if(val.size() >= 2) {
                                        helper::trim(val[1]);
                                        if(helper::is_number(val[1])) {
                                            const int __val__ = std::stoi(val[1]);
                                            if(__val__ < vals.size() && helper::is_number(idk::__to_string__(vals[__val__]))) {
                                                replacement.append(std::to_string(helper::abs(std::stoi(idk::__to_string__(vals[__val__])))));
                                            } else if(__val__ < vals.size()) {
                                                replacement.append(idk::__to_string__(vals[__val__]));
                                            }
                                        }
                                    } else if(count < vals.size()) {
                                        if(helper::is_number(idk::__to_string__(vals[count])))
                                            replacement.append(std::to_string(helper::abs(std::stoi(idk::__to_string__(vals[count++])))));
                                    }
                                }
                            } else if(attr.front() == 'U' || attr.front() == 'L') {
                                auto val = helper::split(attr, ':', __len__);

                                if(val.size() >= 1) {
                                    if(val.size() >= 2) {
                                        helper::trim(val[1]);
                                        if(helper::is_number(val[1])) {
                                            const int __val__ = std::stoi(val[1]);
                                            std::string __data__ = idk::__to_string__(vals[__val__]);

                                            if(__val__ < vals.size() && !helper::is_number(__data__)) {
                                                std::transform(__data__.begin(),
                                                               __data__.end(),
                                                               __data__.begin(), (attr.front() == 'U') ? ::toupper : ::tolower);
                                            } replacement.append(__data__);
                                        }
                                    } else if(count < vals.size()) {
                                        std::string __data__ = idk::__to_string__(vals[count++]);
                                        std::transform(__data__.begin(),
                                                               __data__.end(),
                                                               __data__.begin(), (attr.front() == 'U') ? ::toupper : ::tolower);
                                        replacement.append(__data__);
                                    }
                                }
                            } else if(attr.front() == 'B' || attr.front() == 'Z') {
                                auto val = helper::split(attr, ':', __len__);

                                if(val.size() >= 1) {
                                    if(val.size() >= 2) {
                                        helper::trim(val[1]);
                                        if(helper::is_number(val[1])) {
                                            const int __val__ = std::stoi(val[1]);

                                            if(auto data = idk::__to_string__(vals[__val__]); !helper::is_number(data)) {
                                                replacement.append(data);
                                                break;
                                            }

                                            std::string __bin__ = std::bitset<64>
                                                    (std::stoi(idk::__to_string__(vals[__val__]))).to_string();

                                            if(attr.front() == 'Z') {
                                                __bin__.erase(0, std::min(__bin__.find_first_not_of('0'),
                                                                          __bin__.size() - 1));
                                            }

                                            if(__val__ < vals.size() && helper::is_number(idk::__to_string__(vals[__val__]))) {
                                                replacement.append(__bin__);
                                            } else if(__val__ < vals.size()) {
                                                replacement.append(idk::__to_string__(vals[__val__]));
                                            }
                                        }
                                    } else if(count < vals.size()) {
                                        if(helper::is_number(idk::__to_string__(vals[count]))) {
                                            std::string __bin__ = std::bitset<64>
                                                    (std::stoi(idk::__to_string__(vals[count++]))).to_string();

                                            if(attr.front() == 'Z') {
                                                __bin__.erase(0, std::min(__bin__.find_first_not_of('0'),
                                                                          __bin__.size() - 1));
                                            }

                                            replacement.append(__bin__);
                                        }

                                    }
                                }
                            }
                        }
                    } else { replacement.push_back('{'); } break;
                }

                case '}': {
                    if(i + 1 < data.size()) {
                        if(data[i + 1] == '}') {
                            replacement.push_back('}');
                            ++i;
                        }
                    } else {
                        replacement.push_back('}');
                    } break;
                }

                default: {
                    replacement.push_back(data[i]);
                    break;
                }
            }
        }

        return replacement;
    }
}