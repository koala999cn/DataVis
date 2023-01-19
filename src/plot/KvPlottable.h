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
//    ����Ĭ��z�������
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

	bool forceDefaultZ() const { return forceDefaultZ_; }
	bool& forceDefaultZ() { return forceDefaultZ_; }

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

	float brightenCoeff() const { return brightenCoeff_; }
	float& brightenCoeff() { return brightenCoeff_; }

	auto& colorMappingRange() const { return colorMappingRange_; }
	auto& colorMappingRange() { return colorMappingRange_; }

	unsigned colorMappingDim() const { return colorMappingDim_; }
	void setColorMappingDim(unsigned d); // ��ֵ�ܹؼ���ʹ��set��ֵ

	void resetColorMappingRange();

protected:

	// ȷ�������valpΪ����ԭֵ������ǿ���滻z֮���ֵ
	color4f mapValueToColor_(float_t* valp, unsigned channel) const;

	// ���ݵ�ǰ��coloringMode_������ɫ
	void updateColorMappingPalette_();

	// ����һ����ɢ�������ݶ���
	// ���data()��Ա����Ϊ��ɢ���ݣ���ֱ�ӷ��أ�������sampCount_����������һ����������
	std::shared_ptr<KvDiscreted> discreted_() const;

	// �Ƿ������Ƿ�defaultZ������2������֮һΪtrue��
	// һ��data_��dim����1���������ʼ��ʹ��defaultZ�ȱʧ��zֵ��
	// ����forceDefaultZ_Ϊ��
	bool usingDefaultZ_() const;

	// ����valp����point3����
	// ���forceDefaultZ_Ϊ�棬���滻valp��zֵ��������ԭzֵ
	point3 toPoint_(float_t* valp, unsigned ch) const {
		return { valp[0], valp[1], forceDefaultZ() ? defaultZ(ch) : valp[2] };
	}

private:

	virtual void drawDiscreted_(KvPaint*, KvDiscreted*) const = 0;

private:

	// ��������
	data_ptr data_;

	// ɫ�ʹ���
	KeColoringMode coloringMode_{ k_one_color_solid };
	KtGradient<float_t, color4f> colorBar_; // ɫ��
	bool flatShading_{ false };  // ��true������flat��Ⱦģʽ������ʹ��smooth��Ⱦģʽ
	                             // flatģʽ�£�ʹ�ö���ε����1������������������ɫ

	float brightenCoeff_{ 0.5 }; // ������ǿϵ������������k_one_color_gradiantɫ��ģʽ
	unsigned colorMappingDim_{ 2 }; // ʹ�ø�ά�ȵ����ݽ���ɫ��ӳ��
	std::pair<float_t, float_t> colorMappingRange_; // ɫ��ӳ���ֵ��Χ

	// ��ά�ȵĲ�������Ŀ, ����������������
	std::vector<unsigned> sampCount_{ std::vector<unsigned>({ 1000 }) }; 

	// ���ڷ��������ᣬȱʡΪnull����ʾʹ����������
	std::array<std::unique_ptr<KcAxis>, 3> selfAxes_; 

	// ��Z������⴦����2�����ã�
	// һ�Ƕ���һά���ݣ��ȱʡ��zֵ���Ա���3d�ռ����
	// ���Ƕ��ڸ�ά���ݣ��滻ԭ����zֵ���Ա���3d�ռ�ͶӰ��x-yƽ�����

	float_t defaultZ_{ 0 }; // ��ά���ݵ�z�Ὣ����Ϊ��ֵ
	float_t stepZ_{ 1 }; // ��ͨ����ά���ݵ�z��ƫ�ơ����뽫��ͨ��������ʾ��һ��zƽ�棬�ø�ֵΪ0

	// �ñ��Ϊ��ʱ����ǿ����Ĭ��Zֵ�滻ԭ����zֵ����������3d�ռ���ƶ�ά��colormapͼ
	bool forceDefaultZ_{ false };
};
