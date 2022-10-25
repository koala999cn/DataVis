#pragma once

// 固定数据尺寸的离散余弦变换，基于praat实现
// TODO: test htkCompact
class KgDct
{
public:

	KgDct(unsigned idim, unsigned odim, bool norm = false);

    ~KgDct();

    void forward(const double* in, double* out) const; 
    void backward(const double* in, double* out) const; 

	unsigned idim() const { return idim_; }
	unsigned odim() const { return odim_; }

protected:
	unsigned idim_, odim_;
    double* cosinTable_; // 存储按[odim][idim]布局

	double& operator()(int row, int col) {
		return cosinTable_[row * idim_ + col];
	}
	const double& operator()(int row, int col) const {
		return cosinTable_[row * idim_ + col];
	}

private:

	bool norm_;

	void initCosinTable_();
};

