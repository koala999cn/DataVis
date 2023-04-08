#pragma once
#include "KvRenderable.h"
#include <memory>
#include "KtColor.h"
#include "KtGradient.h"
#include "KcAxis.h"

class KvData;
class KvDiscreted;
class KvPaint;

//
// �ɻ��ƶ���Ļ���. ����ʵ�����¹��ܣ�
//    һ���������ݵĴ洢����������͸��µĹ���
//    ����ɫ��ģʽ����ɫ����
//    ���Ƕ��������ݵĲ�����������ֻ�������ɢ����
//    ���Ƿ�������������úʹ洢
//    ����Ĭ��z�������
//    �������ݵ���ɫ�ı��״̬׷��

class KvPlottable : public KvRenderable
{
	using super_ = KvRenderable;

public:
	using data_ptr = std::shared_ptr<KvData>;
	using const_data_ptr = std::shared_ptr<const KvData>;
	using point3 = KtPoint<float_t, 3>;
	using gradient_t = KtGradient<float, color4f>;

	KvPlottable(const std::string_view& name);

	bool empty() const; // �������ݣ�������Ϊ�գ�����true

	const_data_ptr idata() const { return data_; }

	virtual void setData(const_data_ptr d);
	virtual const_data_ptr odata() const { return data_; }

	// ��֤odata��δ����ʱҲ�ܻ�ȡ������ݵ�ά�ȣ��Ա�ͬ����س�Ա
	// ȱʡʵ�ַ����������ݵ�ά�ȣ�����������Ϊ�գ��򷵻�0
	virtual unsigned odim() const;

	// ��plt��¡��������
	virtual void cloneConfig(const KvPlottable& plt);

	unsigned sampCount(unsigned dim) const { return sampCount_[dim]; }
	void setSampCount(unsigned dim, unsigned c);

	// ���õ�dimά�ȵķ���������
	void setAxis(unsigned dim, const std::shared_ptr<KcAxis>& axis) { selfAxes_[dim] = axis; }

	// ����dimά�ȵķ�������������
	std::shared_ptr<KcAxis> axis(unsigned dim) const { return selfAxes_[dim]; }

	// ��plt�Ƿ��з���������
	bool hasSelfAxis() const;

	aabb_t boundingBox() const final;

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
	virtual void setMinorColor_(const color4f& minor) = 0; 
	void setMinorColor(const color4f& minor); // ����coloringChanged״̬

	// ����ɫ���Ķ�д�ӿ�
	const gradient_t& gradient() const { return grad_; }
	void setGradient(const gradient_t& grad);

	////////////////////////////////////////////////////////////////

	/// zֵ����

	float_t defaultZ() const { return defaultZ_; }
	void setDefaultZ(float_t z);

	float_t stepZ() const { return stepZ_; }
	void setStepZ(float_t step);

	bool forceDefaultZ() const { return forceDefaultZ_; }
	void setForceDefaultZ(bool b);

	float_t defaultZ(unsigned ch) const { return defaultZ_ + ch * stepZ_; }

	////////////////////////////////////////////////////////////////

	/// ɫ��ģʽ֧��

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
	void setFlatShading(bool b);

	float brightenCoeff() const { return brightenCoeff_; }
	void setBrightenCoeff(float coeff);

	auto& colorMappingRange() const { return colorMappingRange_; }
	void setColorMappingRange(const std::pair<float_t, float_t>& r);

	unsigned colorMappingDim() const { return colorMappingDim_; }
	void setColorMappingDim(unsigned d); // ��ֵ�ܹؼ���ʹ��set��ֵ

	void fitColorMappingRange();

	bool autoColorMappingRange() const { return autoColorMappingRange_; }
	bool& autoColorMappingRange() { return autoColorMappingRange_; };

	// ���ݵ�ǰ��coloringMode_������ɫ
	void updateColorMappingPalette();


	/// VBO���ýӿ�

public:

	bool dataChanged() const { return dataChanged_; }

	void setDataChanged(bool reoutput) { 
		if (dataChanged_ < 1 + reoutput)
		    dataChanged_ = 1 + reoutput; 
	}

	int coloringChanged() const { return coloringChanged_; }

	unsigned objectsReused() const { return objectsReused_; }

	// ����plt��������Ⱦ��������
	virtual unsigned objectCount() const = 0;

protected:

	// ����true��ʾִ�������ݸ��²���
	virtual bool output_();

	virtual void outputImpl_() {}

	virtual bool objectVisible_(unsigned objIdx) const = 0;

	// ���õ�objIdx����Ⱦ�������Ⱦ״̬
	virtual void setObjectState_(KvPaint*, unsigned objIdx) const = 0;

	// ��objIdx����Ⱦ�����Ƿ������
	virtual bool objectReusable_(unsigned objIdx) const;

	// ���Ƶ�objIdx����Ⱦ���󣬲����ؿɸ��õĶ���id
	virtual void* drawObject_(KvPaint*, unsigned objIdx) const = 0;

	mutable std::vector<void*> renderObjs_; // KvPaint���ص���Ⱦ����id������vbo����
	mutable unsigned objectsReused_{ 0 }; // ����vbo�Ķ�������, for debug

	//////////////////////////////////////////////////////////////////////


protected:

	virtual aabb_t calcBoundingBox_() const;

	// ȷ�������valpΪ����ԭֵ������ǿ���滻z֮���ֵ
	color4f mapValueToColor_(const float_t* valp, unsigned channel) const;

	// ����һ����ɢ����������ݶ���
	// ���odata()��Ա����Ϊ��ɢ���ݣ���ֱ�ӷ��أ�������sampCount_����������һ����������
	std::shared_ptr<const KvDiscreted> discreted_() const;

	// ����valp����point3����
	// ���forceDefaultZ_Ϊ�棬���滻valp��zֵ��������ԭzֵ
	point3 toPoint_(const float_t* valp, unsigned ch) const;

	// �Ƿ������Ƿ�defaultZ������2������֮һΪtrue��
	// һ��data_��dim����1���������ʼ��ʹ��defaultZ�ȱʧ��zֵ��
	// ����forceDefaultZ_Ϊ��
	bool usingDefaultZ_() const;

	void setBoundingBoxExpired_() const { box_.setNull(); }

private:

	// ��������
	const_data_ptr data_;

	// ɫ�ʹ���
	KeColoringMode coloringMode_{ k_one_color_solid };
	gradient_t grad_; // ����ɫ��
	bool flatShading_{ false };  // ��true������flat��Ⱦģʽ������ʹ��smooth��Ⱦģʽ
	                             // flatģʽ�£�ʹ�ö���ε����1������������������ɫ

	float brightenCoeff_{ 0.5 }; // ������ǿϵ������������k_one_color_gradiantɫ��ģʽ
	unsigned colorMappingDim_{ 2 }; // ʹ�ø�ά�ȵ����ݽ���ɫ��ӳ��

	std::pair<float_t, float_t> colorMappingRange_; // ɫ��ӳ���ֵ��Χ
	bool autoColorMappingRange_{ true }; // �Ƿ�����Ӧɫ��ӳ�䷶Χ

	// ��ά�ȵĲ�������Ŀ, ����������������
	std::vector<unsigned> sampCount_{ std::vector<unsigned>({ 1000 }) }; 

	// ���ڷ��������ᣬ��null�����ڼ���plot��ʱ����Ϊ��������
	std::array<std::shared_ptr<KcAxis>, 3> selfAxes_; 

	// ��Z������⴦����2�����ã�
	// һ�Ƕ���һά���ݣ��ȱʡ��zֵ���Ա���3d�ռ����
	// ���Ƕ��ڸ�ά���ݣ��滻ԭ����zֵ���Ա���3d�ռ�ͶӰ��x-yƽ�����

	float_t defaultZ_{ 0 }; // ��ά���ݵ�z�Ὣ����Ϊ��ֵ
	float_t stepZ_{ 1 }; // ��ͨ����ά���ݵ�z��ƫ�ơ����뽫��ͨ��������ʾ��һ��zƽ�棬�ø�ֵΪ0

	// �ñ��Ϊ��ʱ����ǿ����Ĭ��Zֵ�滻ԭ����zֵ����������3d�ռ���ƶ�ά��colormapͼ
	bool forceDefaultZ_{ false };

	mutable int dataChanged_{ 0 }; // 0��ʾδ���ڣ�1��ʾ���ڵ��Ѹ��£�2��ʾ������δ����
	mutable int coloringChanged_{ 0 }; // 0��ʾ�ޱ仯��1��ʾС�仯(colorful֮��ı仯)��2��ʾ��仯(solid <-> colorful)

	mutable aabb_t box_; // �����aabb������Ⱦ��������ʱ�������Ч�ʣ�autofit����£�
};
