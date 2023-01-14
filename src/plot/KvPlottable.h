#pragma once
#include "KvRenderable.h"
#include <memory>
#include "KtColor.h"
#include "KtGradient.h"
#include "KcAxis.h"

class KvData;
class KvDiscreted;

//
// �ɻ��ƶ���Ļ���. ����ʵ�����¹��ܣ�
//    һ�ǻ������ݵĴ洢
//    ����ɫ��ģʽ����ɫ����ɫ����
//    ���Ƕ��������ݵĲ�����������ֻ�������ɢ����
//    ���Ƿ�������������úʹ洢
//    ���Ƕ�ά���ݵ�ȱʡz��������
//

class KvPlottable : public KvRenderable
{
	using super_ = KvRenderable;

public:
	using data_ptr = std::shared_ptr<KvData>;
	using point3 = KtPoint<float_t, 3>;

	KvPlottable(const std::string_view& name);

	bool empty() const; // �������ݣ�������Ϊ�գ�����true

	data_ptr data() const { return data_; }
	void setData(data_ptr d);

	unsigned sampCount(unsigned dim) const { return sampCount_[dim]; }
	unsigned& sampCount(unsigned dim) { return sampCount_[dim]; }

	// ���õ�dimά�ȵķ��������ᣬ�ú����ӹ�axis�Ŀ���Ȩ
	void setAxis(unsigned dim, KcAxis* axis) { selfAxes_[dim].reset(axis); }

	// ����dimά�ȵķ�������������
	KcAxis* axis(unsigned dim) const { return selfAxes_[dim].get(); }

	// ��plt�Ƿ��з���������
	bool hasSelfAxis() const;

	aabb_t boundingBox() const override;

	// ��װ�������ݵĻ��ƣ��ṩ����һ��������ɢ���ݵĽӿ�drawDiscreted_
	void draw(KvPaint*) const override;


	/// ����Ϊ��ɫ��ͨ�ýӿ� //////////////////////////////////

	/// ÿ��plottable������������ɫ��1����ɫ
    /// ��ɫ�Ĺ���������colorBar_����ʵ�֣���ɫ�Ĺ��������û�����ʵ��

	// ���ش�plottable��Ҫ����ɫ��Ŀ������-1��ʾ��Ҫ����ɫ��
	// �ṩȱʡʵ�֣���KeColoringModeΪk_coloring_flat_shading��k_coloring_smooth_shadingʱ������-1
	// ���򷵻����ݵ�ͨ����Ŀ����ÿ��ͨ������1����ɫ
	virtual unsigned majorColorsNeeded() const;

	// ������ɫ
	// �ṩȱʡʵ�֣���majors����洢ΪcolorBar_��Ա�Ŀ���ɫ
	// �û������ظú�������ͬ���洢�������ط�����pen��brush�������ģ���ɫ��ֵ
	virtual void setMajorColors(const std::vector<color4f>& majors);

	// ��ɫ�ʵĸ�����һ�����majorColorNeeded��
	// ������ɫ������£�����������ɫ�ĸ���
	// �ṩȱʡʵ�֣�����colorBar_�Ŀ���ɫ��Ŀ
	virtual unsigned majorColors() const;

	// ���ص�idx����ɫ
	// assert(idx < majorColors())
	// �ṩȱʡʵ�֣�����colorBar_�ĵ�idx������ɫ
	virtual color4f majorColor(unsigned idx) const;


	// ��ɫ�洢���������û�ʵ��
	virtual const color4f& minorColor() const = 0;
	virtual void setMinorColor(const color4f& minor) = 0;

	////////////////////////////////////////////////////////////////

	float_t defaultZ() const { return defaultZ_; }
	float_t& defaultZ() { return defaultZ_; }

	float_t stepZ() const { return stepZ_; }
	float_t& stepZ() { return stepZ_; }

	float_t defaultZ(unsigned ch) const { return defaultZ_ + ch * stepZ_; }

	////////////////////////////////////////////////////////////////

	// ɫ��ģʽ֧��

	enum KeColoringMode
	{
		k_one_color_solid, // ��ɫ
		k_one_color_gradiant, // ��ɫ���Ƚ���
		k_two_color_gradiant, // ˫ɫ����ɫ�͸�ɫ������

		// ����ģʽmajorColorsNeeded����-1����ʾ��Ҫɫ��֧��
		// ����ģʽ�£���ͨ������1��ɫ��
		k_colorbar_gradiant, // ��ɫ����
	};

	KeColoringMode coloringMode() const { return coloringMode_; }
	void setColoringMode(KeColoringMode mode);

	bool flatShading() const { return flatShading_; }
	bool& flatShading() { return flatShading_; }

	auto& colorMappingRange() const { return colorMappingRange_; }
	auto& colorMappingRange() { return colorMappingRange_; }

	void fitColorMappingRange();

protected:

	color4f mapValueToColor_(float_t val, unsigned channel) const;

private:

	virtual void drawDiscreted_(KvPaint*, KvDiscreted*) const = 0;

private:

	// ��������
	data_ptr data_;

	// ɫ�ʹ���
	KeColoringMode coloringMode_{ k_one_color_solid };
	KtGradient<float_t, color4f> colorBar_; // ɫ��
	std::pair<float_t, float_t> colorMappingRange_; // ɫ��ӳ���ֵ��Χ
	bool flatShading_{ false };  // ��true������flat��Ⱦģʽ������ʹ��smooth��Ⱦģʽ
	                             // flatģʽ�£�ʹ�ö���εĵ�1������������������ɫ

	// ��ά�ȵĲ�������Ŀ, ����������������
	std::vector<unsigned> sampCount_{ std::vector<unsigned>({ 1000 }) }; 

	// ���ڷ��������ᣬȱʡΪnull����ʾʹ����������
	std::array<std::unique_ptr<KcAxis>, 3> selfAxes_; 

	// ���³�Ա���Զ�ά������Ч
	float_t defaultZ_{ 0 }; // ��ά���ݵ�z�Ὣ����Ϊ��ֵ
	float_t stepZ_{ 1 }; // ��ͨ����ά���ݵ�z��ƫ�ơ����뽫��ͨ��������ʾ��һ��zƽ�棬�ø�ֵΪ0
};
