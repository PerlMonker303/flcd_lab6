#include "Helper.h"

std::vector<std::string> Helper::splitString(std::string str, char delim) {
    std::vector<std::string> result;
    std::istringstream iss(str);
    std::string item;
    while (std::getline(iss, item, delim)) {
        result.push_back(item);
    }
    return result;
}

bool Helper::findInVector(std::vector<std::string> vec, std::string elem) {
    for (auto v : vec) {
        if (v == elem) {
            return true;
        }
    }
    return false;
}

bool Helper::findInSet(std::unordered_set<std::string> set, std::string elem) {
    for (auto s : set) {
        if (s == elem) {
            return true;
        }
    }
    return false;
}

void Helper::removeElementFromSet(std::unordered_set<std::string>& set, std::string elem) {
    std::unordered_set<std::string> result;
    for (auto s : set) {
        if (s != elem) {
            result.insert(s);
        }
    }
    set = result;
}

bool Helper::isAllCaps(std::string s) {
    if (s.length() == 0) {
        return false;
    }
    for (auto c : s) {
        if (c < 'A' || c > 'Z') {
            return false;
        }
    }
    return true;
}

std::unordered_set<std::string> Helper::concatenationOne(std::unordered_set<std::string> set1, std::unordered_set<std::string> set2, bool& error) {
    std::unordered_set<std::string> result;
    if (set1.empty()) {
        error = true;
        return result;
    }
    if (set2.empty()) {
        return set1;
    }
    for (auto el1 : set1) {
        if (el1 == "Epsilon") {
            for (auto el2 : set2) {
                result.insert(el2);
            }
        }
        else {
            for (auto el2 : set2) {
                result.insert(el1);
            }
        }
    }
    return result;
}

std::unordered_set<std::string> Helper::union_custom(std::unordered_set<std::string> set1, std::unordered_set<std::string> set2, bool noEpsilon) {
    std::unordered_set<std::string> result;
    for (auto el1 : set1) {
        result.insert(el1);
    }
    for (auto el2 : set2) {
        if (noEpsilon && el2 == "Epsilon") {
            continue;
        }
        result.insert(el2);
    }
    return result;
}

std::unordered_set<std::string> Helper::intersect_custom(std::unordered_set<std::string> set1, std::unordered_set<std::string> set2) {
    std::unordered_set<std::string> result;
    for (auto el1 : set1) {
        if (set2.find(el1) != set2.end()) {
            result.insert(el1);
        }
    }
    return result;
}