#pragma once
#include <vector>
#include "KtFraming.h"


// FIR�˲�����ģ����
// @TD: ����������ݵ�����
// @TC: �˲���ϵ��������

template<typename TD, typename TC = TD>
class KtFIR
{
public:
	// ��ͨ����ÿ��ƽ��1������
	KtFIR(std::vector<TC>&& h, unsigned channels = 1) : h_(std::move(h)), buf_(h.size(), channels, 1, 0) {}
	KtFIR(const std::vector<TC>& h, unsigned channels = 1) : h_(h), buf_(h.size(), channels, 1, 0) {}


	// �˲�������
	auto order() const { return h_.size() - 1; }

	// �˲�����ͷ��
	auto taps() const { return h_.size(); }

	unsigned channels() const { return buf_.channels(); }


	// ������inִ���˲�����������洢��out������д���������
	// out�ĳߴ粻С��count * channels
	unsigned apply(const TD* in, unsigned count, TD* out);

	// �Ի�������ִ���˲�����������洢��out������д���������
	// out�ĳߴ粻С��count * channels
	unsigned flush(TD* out);


private:
	struct KpFilter
	{
		KpFilter(std::vector<TC>& h_, TD*& out_, unsigned ch_) : h(h_), out(out_), ch(ch_) {}

		void operator()(const TD* data) {
			for (unsigned c = 0; c < ch; c++) {
				TD v{0};
				const TD* dp = data + c;
				const TC* hp = h.data();
				for (unsigned i = 0; i < h.size(); i++) {
					v += *dp * *hp++; // ���
					dp += ch;
				}
				*out++ = v;
			}
		}

		std::vector<TC>& h;
		TD*& out;
		unsigned ch;
	};

private:
	std::vector<TC> h_; // �˲�ϵ��
	KtFraming<TD> buf_;
};


template<typename TD, typename TC>
unsigned KtFIR<TD, TC>::apply(const TD* in, unsigned count, TD* out)
{
	auto buf = out;
	KpFilter op(h_, buf, channels());
	buf_.apply(in, in + count, op);
	return (buf - out) / channels();
}


template<typename TD, typename TC>
unsigned KtFIR<TD, TC>::flush(TD* out)
{
	auto buf = out;
	KpFilter op(h_, buf, channels());
	buf_.flush(op);
	return (buf - out) / channels();
}

