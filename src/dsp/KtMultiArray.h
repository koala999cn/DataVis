#pragma once
#include <blitz/array.h>

//
// ����blitz�Ķ�ά����ʵ��
// @T: ����Ԫ�ص�����
// @DIM: �����ά�ȣ�����>0
//

template<typename T, int DIM>
class KtMultiArray
{
    static_assert(DIM > 0);

public:
    KtArray() = default;

    // ÿ��ͨ������������
    kIndex size() const override {
        return channels() == 0 ? 0 : array_.size() / channels();
    }

    auto size(int axis) const {
        return array_.length(axis);
    }

    void clear() {
        array_.resize(0); 
    }

    void resize(int shape[]) {
        array_.resizeAndPreserve(makeTinyVector_(shape, chs));
    }

    const T& at(int idx[]) const {
        return array_(makeTinyVector_(idx));
    }


    /// �й�д�����ĳ�Ա����

    T* data() { return array_.dataFirst(); }
    const T* data() const { return array_.dataFirst(); }

    T* row(int idx) { return &array_(idx); }
    const T* row(int idx) const { return &array_(idx); }

    auto stride(int axis) const { return array_.stride(axis); }

    // ֱ�Ӵ�data�����������ײ�����
    void fill(const kReal * data) {
        std::copy(data, data + array_.size(), array_.dataFirst());
    }


    // ɾ��ǰrows��
    //void popFront(kIndex rows);

    // �Ƴ���0����ֵС��x��rows
    //void cutBefore(kReal x);

private:
    static auto makeTinyVector_(int idx[]) {
        blitz::TinyVector<int, DIM> tv;
        std::copy(idx, idx + DIM, tv.begin());
        return tv;
    }

private:
    blitz::Array<T, DIM> array_;
};

