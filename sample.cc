#include "ChisTrie.h"
#include <iostream>
using namespace std;
void wchar_test() {
    chis::lctrie<wstring, wstring, chis::Trie_decoder_ONESTEP> trie;
    wcout.imbue(locale(locale(),"",LC_CTYPE)); 
    {//一波初始化
        vector<pair<wstring, wstring>> words = {
            {L"李", L"姓"}, 
            {L"李刚", L"爸爸"}, 
            {L"李天一", L"儿子"},
            {L"刚", L"爸爸的名字"},
            {L"天一", L"儿子的名字"}, 
            {L"刚仔", L"爸爸的昵称1"}, 
            {L"刚刚", L"爸爸的昵称2"}, 
            {L"仔", L"儿子的昵称1"}, 
            {L"仔仔", L"儿子的昵称2"},
            {L"天一的爸", L"李刚"},
            {L"我", L"就是我，是颜色不一样的烟火"},
            {L"的爸", L"的哥的爸爸"},
            };
        for(auto w:words) {
            trie.insert(w.first, w.second);
        }
        trie.build_aho_corasick();//构建ac自动机
    }
    wstring input = L"我李天一的爸是李刚刚仔仔";
    auto res = trie.match(input);//默认返回左优先的贪心匹配
    for(auto r:res) {
        wcout << input.substr(0, r.begin-input.c_str()) 
        << L"[" << input.substr(r.begin-input.c_str(),r.end-r.begin) << L"(" << r.val << L")] "
        << input.substr(r.end-input.c_str()) << endl;
    }
    wcout << "//////////////+Full Match//////////////" << endl;
    res = trie.match(input, true);//返回所有结果
    for(auto r:res) {
        wcout << input.substr(0, r.begin-input.c_str()) 
        << L"[" << input.substr(r.begin-input.c_str(),r.end-r.begin) << L"(" << r.val << L")]"
        << input.substr(r.end-input.c_str()) << endl;
    }
    wcout << "//////////////-Longest Match//////////////" << endl;
    res = trie.match(input, false, false);//非贪心匹配
    for(auto r:res) {
        wcout << input.substr(0, r.begin-input.c_str()) 
        << L"[" << input.substr(r.begin-input.c_str(),r.end-r.begin) << L"(" << r.val << L")]"
        << input.substr(r.end-input.c_str()) << endl;
    }
}
void norm_test() {
    chis::lctrie<string, string, chis::Trie_decoder_UTF8> trie;
    {//一波初始化
        vector<pair<string, string>> words = {
            {"李", "姓"}, 
            {"李刚", "爸爸"}, 
            {"李天一", "儿子"},
            {"刚", "爸爸的名字"},
            {"天一", "儿子的名字"}, 
            {"刚仔", "爸爸的昵称1"}, 
            {"刚刚", "爸爸的昵称2"}, 
            {"仔", "儿子的昵称1"}, 
            {"仔仔", "儿子的昵称2"},
            {"天一的爸", "李刚"},
            {"我", "就是我，是颜色不一样的烟火"},
            {"的爸", "的哥的爸爸"},
            };
        for(auto w:words) {
            trie.insert(w.first, w.second);
        }
        trie.build_aho_corasick();//构建ac自动机
    }
    string input = "我李天一的爸是李刚刚仔仔";
    auto res = trie.match(input);//默认返回左优先的贪心匹配
    for(auto r:res) {
        cout << input.substr(0, r.begin-input.c_str()) 
        << "[" << input.substr(r.begin-input.c_str(),r.end-r.begin) << "(" << r.val << ")] "
        << input.substr(r.end-input.c_str()) << endl;
    }
    cout << "//////////////+Full Match//////////////" << endl;
    res = trie.match(input, true);//返回所有结果
    for(auto r:res) {
        cout << input.substr(0, r.begin-input.c_str()) 
        << "[" << input.substr(r.begin-input.c_str(),r.end-r.begin) << "(" << r.val << ")]"
        << input.substr(r.end-input.c_str()) << endl;
    }
    cout << "//////////////-Longest Match//////////////" << endl;
    res = trie.match(input, false, false);//非贪心匹配
    for(auto r:res) {
        cout << input.substr(0, r.begin-input.c_str()) 
        << "[" << input.substr(r.begin-input.c_str(),r.end-r.begin) << "(" << r.val << ")]"
        << input.substr(r.end-input.c_str()) << endl;
    }
}
int main() {
    norm_test();
    cout << "///////////////// wstring ////////////////" << endl;
    wchar_test();
    return 0;
}
