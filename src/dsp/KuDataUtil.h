#pragma once
#include <memory>
#include <vector>
#include <functional>
#include <assert.h>

class KvData;
class KvDiscreted;

class KuDataUtil
{
public:
	using matrixd = std::vector<std::vector<double>>;

	enum KeDataType
	{
		k_series,
		k_matrix,
		k_scattered_1d,
		k_scattered_2d,
		k_sampled_1d,
		k_sampled_2d
	};

	// ���mat��ת������������
    // ����[0]Ϊ�Ƽ�����
	static std::vector<int> validTypes(const matrixd& mat, bool transpose);

	static std::shared_ptr<KvData> makeSeries(const matrixd& mat);

	static std::shared_ptr<KvData> makeMatrix(const matrixd& mat);

	// ��ͨ����һά��������
	static std::shared_ptr<KvData> makeSampled1d(const matrixd& mat);

	// ��ͨ���Ķ�ά��������
	static std::shared_ptr<KvData> makeSampled2d(const matrixd& mat);

	static std::shared_ptr<KvData> makeScattered(const matrixd& mat, unsigned dim);

	// ��matת��Ϊtype��������
	// ����row-major��ȡmat����
	static std::shared_ptr<KvData> makeData(const matrixd& mat, int type);

	static std::shared_ptr<KvData> cloneSampled1d(std::shared_ptr<KvData> samp);

	static bool isMatrix(const KvData& d);
	
	//static bool sameShape(const KvData& d1, const KvData& d2);

	//static void reshapeAs(KvData& from, const KvData& to);

	// shape���������һ�£���������
	// static std::shared_ptr<KvData> emptyLike();
	
	// shape���������һ�£�������������0
	// static std::shared_ptr<KvData> zeroLike();

	// shape�������������������ݾ�һ��
	// static std::shared_ptr<KvData> sameLike();


	/// ���ݷ��ʽӿ�

	struct KpValueGetter1d
	{
		std::function<double(unsigned ch, unsigned idx)> getter{ nullptr };
		unsigned channels{ 0 }; // ͨ����
		unsigned samples{ 0 }; // ÿ��ͨ����������
		const double* data{ nullptr }; // �ڴ���ʵ�ַ��nullptr��ʾ�����ڴ���ʣ�
		int channelStride{ 0 }; // ��ͨ����Ŀ�ȣ�double����������data�ǿ�ʱ��Ч
		int sampleStride{ 0 }; // ��������Ŀ�ȣ�double����������data�ǿ�ʱ��Ч

		// ��ȡָ��ͨ������
		auto fetchChannel(unsigned ch, unsigned offset, unsigned count) const {
			assert((offset < samples || count == 0) && count <= samples - offset);
			std::vector<double> buf(count);
			auto p = buf.data();
			auto last = offset + count;
			for (unsigned i = offset; i < last; i++)
				*p++ = getter(ch, i);
			return buf;
		}

		// ��ȡ����ͨ������
		auto fetch(unsigned offset, unsigned count) const {
			assert(offset < samples && count <= samples - offset);
			std::vector<double> buf(count * channels);
			auto p = buf.data();
			auto last = offset + count;
			for (unsigned i = offset; i < last; i++)
				for (unsigned ch = 0; ch < channels; ch++) // ��ͨ�����ݽ���洢
				    *p++ = getter(ch, i);
			return buf;
		}
	};


	static KpValueGetter1d valueGetter1d(const std::shared_ptr<KvData>& data);


	struct KpValueGetter2d
	{
		std::function<double(unsigned ch, unsigned ix, unsigned iy)> getter{ nullptr };
		unsigned channels{ 0 }; // ͨ����
		unsigned xsize{ 0 }; // ÿ��ͨ��������
		unsigned ysize{ 0 }; // ÿ��ͨ��������
		const double* data{ nullptr }; // �ڴ���ʵ�ַ��nullptr��ʾ�����ڴ���ʣ�
		int channelStride{ 0 }; // ��ͨ����Ŀ�ȣ�double����������data�ǿ�ʱ��Ч
		int xstride{ 0 }; // ���м�Ŀ�ȣ�double����������data�ǿ�ʱ��Ч
		int ystride{ 0 }; // ���м�Ŀ�ȣ�double����������data�ǿ�ʱ��Ч

		auto fetchXOfChannel(unsigned ch, unsigned ix) const {
			std::vector<double> buf(ysize);
			auto p = buf.data();
			for (unsigned iy = 0; iy < ysize; iy++)
				*p++ = getter(ch, ix, iy);
			return buf;
		}

		auto fetchX(unsigned ix) const {
			std::vector<double> buf(ysize * channels);
			auto p = buf.data();
			for (unsigned iy = 0; iy < ysize; iy++)
				for (unsigned ch = 0; ch < channels; ch++) // ��ͨ�����ݽ���洢
					*p++ = getter(ch, ix, iy);
			return buf;
		}
	};


	static KpValueGetter2d valueGetter2d(const std::shared_ptr<KvData>& data);

	struct KpPointGetter1d
	{
		std::function<std::vector<double>(unsigned idx)> getter;
		unsigned size;
	};

	struct KpPointGetter2d
	{
		std::function<std::vector<double>(unsigned ix, unsigned iy)> getter;
		unsigned xsize, ysize;
	};

	static bool hasPointGetter2d(const std::shared_ptr<KvDiscreted>& disc);

	// ����discÿ��ͨ���ж�����pointGetter1d
	static unsigned pointGetter1dCount(const std::shared_ptr<KvDiscreted>& disc);

	// ����discÿ��ͨ���ж�����pointGetter2d
	static unsigned pointGetter2dCount(const std::shared_ptr<KvDiscreted>& disc);

	// ��ȡdisc�ĵ�chͨ���ĵ�idx��pointGetter1d
	static KpPointGetter1d pointGetter1dAt(const std::shared_ptr<KvDiscreted>& disc, unsigned ch, unsigned idx);

	// ��ȡdisc�ĵ�chͨ���ĵ�idx��pointGetter2d
	static KpPointGetter2d pointGetter2dAt(const std::shared_ptr<KvDiscreted>& disc, unsigned ch, unsigned idx);
	

private:
	KuDataUtil() = default;
};
