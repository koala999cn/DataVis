#pragma once
#include <atomic>


// 为对象OBJ_TYPE生成类型为ID_TYPE的唯一标识符
// ID_TYPE必须能执行++运算符

template<typename OBJ_TYPE, typename ID_TYPE = int>
class KtUId
{
public:

    constexpr KtUId() : id_{ newId_() } {}

    KtUId(const KtUId&) = delete;

    KtUId(KtUId&& rhs) noexcept : m_id{ rhs.id() } {
        rhs.reset();
    }

    KtUId& operator=(KtUId&& rhs) noexcept {
        id_ = rhs.id();
        rhs.reset();
        return *this;
    }

    ID_TYPE id() const { return id_; }

    void reset() { id_ = ID_TYPE{ 0 }; }

private:

    static ID_TYPE newId_() {
        static std::atomic<ID_TYPE> counter{ 0 };
        return ++counter;
    }

private:
    ID_TYPE id_{ 0 };
};