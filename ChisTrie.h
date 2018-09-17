#pragma once
#include <map>
#include <list>
#include <queue>
#include <string>
namespace chis {
	enum STATUS { LEAF, INTER, ROOT };
	struct Trie_decoder_GBK { //Tire树编码类，next返回begin的后继和当前字符的offset
		inline static std::pair<const char*, size_t> next(const char* begin, const char* end);
	};
	struct Trie_decoder_UTF8 {
		inline static std::pair<const char*, size_t> next(const char* begin, const char* end);
	};
	template<class _charTy, class _valTy>
	struct _trie_node {
		_trie_node() {}
		_trie_node(STATUS status) :m_status(status) {}
		std::map<_charTy, _trie_node*> m_succTo;//匹配跳转
		STATUS m_status;
		_valTy m_val;
		_trie_node *m_failTo = nullptr;//失配跳转
		size_t depth = 0;
	};
	template<class _valTy, class _keyItor>
	struct _match_res {
		_valTy val;
		_keyItor begin;
		_keyItor end;
	};

	template<class _valTy = std::string, class Trie_decoder = Trie_decoder_GBK>
	class lctrie {
		using _charTy = std::string;
		using _keyItor = const char*;
	public:
		lctrie() {
			m_allNode.emplace_back(ROOT);
			m_root = &m_allNode.back();
		};
		std::vector<_match_res<_valTy, _keyItor>> match(const std::string &str, bool fullMatch = false, bool longest = true) const {
			std::vector<_match_res<_valTy, _keyItor>> ret;
			auto node = m_root;
			const char *begin = str.c_str(), *end = str.c_str() + str.size();
			auto next = Trie_decoder::next(begin, end);
			auto nextchr = std::string(begin, next.first);
			while (begin != end || node != m_root) {
				do {
					if (node->m_succTo.count(nextchr)) {//可行跳转
						node = node->m_succTo[nextchr];
					} else if (node != m_root) {//非可行跳转，尝试跳转到fail指针
						node = node->m_failTo ? node->m_failTo : m_root;
						continue;//重新匹配nextchr
					}
					begin = next.first;
					next = Trie_decoder::next(begin, end);
					nextchr = std::string(begin, next.first);
				} while (node->m_status != LEAF && (begin != end || node != m_root));
				if (node->m_status == LEAF) {
					auto mbegin = begin - node->depth;
					auto mend = begin;
					if (!fullMatch && !ret.empty()) {
						if (longest && ret.back().begin == mbegin) {//贪心匹配(留下长结果)
							ret.back().end = mend;
							ret.back().val = node->m_val;
						} else if (ret.back().end <= mbegin) { //相交过滤
							ret.push_back({ node->m_val, mbegin, mend });
						}
					} else { //全匹配(包括包含，相交结果)
						ret.push_back({ node->m_val, mbegin, mend });
					}
				}
			}
			return ret;
		}
		lctrie& insert(const std::string &str, const _valTy &val) {
			acIsBuild = false;
			auto node = m_root;
			const char *begin = str.c_str(), *end = str.c_str() + str.size();
			while (begin != end) {
				auto next = Trie_decoder::next(begin, end);
				auto nextchr = std::string(begin, next.first);
				begin = next.first;
				if (!node->m_succTo.count(nextchr)) { //没有这个分支，增加节点
					m_allNode.emplace_back(INTER);
					auto &back = m_allNode.back();
					back.depth = begin-str.c_str();
					node->m_succTo[nextchr] = &back;
				}
				node = node->m_succTo[nextchr];
				
			}
			node->m_val = val;
			node->m_status = LEAF;
			return *this;
		}
		lctrie& build_aho_corasick() {
			if (acIsBuild) {
				return *this;
			}
			acIsBuild = true;
			std::queue<_trie_node<_charTy, _valTy>*> quNode;
			for (auto succNext : m_root->m_succTo) {
				auto next = succNext.second;
				auto nextchr = succNext.first;
				next->m_failTo = m_root;
				quNode.push(next);
			}
			while (!quNode.empty()) {
				//出队列
				auto tmpPtr = quNode.front();
				quNode.pop();
				//遍历succ边
				for (auto succNext : tmpPtr->m_succTo) {
					auto next = succNext.second;
					auto nextchr = succNext.first;
					//从父节点的fail指针开始，遍历可跳转的fail节点
					auto ptr = tmpPtr->m_failTo;
					while (ptr) {
						//找到可以接受nextchr的fail节点
						if (ptr->m_succTo.count(nextchr)) {
							next->m_failTo = ptr->m_succTo[nextchr];
							break;
						}
						ptr = ptr->m_failTo;
					}
					//没有找到fail指针，则指向m_root
					if (!ptr) {
						next->m_failTo = m_root;
					}
					quNode.push(next);
				}
			}
			return *this;
		}
	private:
		bool acIsBuild = false;
		std::list<_trie_node<_charTy, _valTy>> m_allNode;
		_trie_node<_charTy, _valTy> *m_root;
	};
	std::pair<const char*, size_t> Trie_decoder_GBK::next(const char* begin, const char* end) {
		if (begin < end) {
			unsigned char uc = (unsigned char)*begin;
			size_t offset = 1;
			//双/四字节
			if (uc >= 0x81 && uc <= 0xFE) {
				if (begin + 1 < end) {
					unsigned char tc = (unsigned char)*(begin + 1);
					if (tc >= 0x40 && tc <= 0xFE && tc != 0x7F) {
						offset = 2;//双字节
					} else if (tc >= 0x30 && tc <= 0x39 && begin + 3 < end) {
						unsigned char thrc = (unsigned char)*(begin + 2);
						unsigned char fourc = (unsigned char)*(begin + 3);
						if (thrc >= 0x81 && thrc <= 0xFE && fourc >= 0x30 && fourc <= 0x39) {
							offset = 4;//四字节
						}
					}
				}
			}
			if (begin + offset <= end) {
				return { begin + offset, offset };
			} else {
				return { end, offset };
			}
		}
		return { begin, 0 };
	}
	std::pair<const char*, size_t> Trie_decoder_UTF8::next(const char* begin, const char* end) {
		if (begin < end) {
			unsigned char uc = (unsigned char)*begin;
			size_t offset = 1;
			if ((uc & 0xC0) == 0xC0) {
				offset = 2;
			} else if ((uc & 0xE0) == 0xE0) {
				offset = 3;
			} else if ((uc & 0xF0) == 0xF0) {
				offset = 4;
			} else if ((uc & 0xF8) == 0xF8) {
				offset = 5;
			}
			if (begin + offset <= end) {
				return { begin + offset, offset };
			} else {
				return { end, offset };
			}
		}
		return { begin, 0 };
	}
}
