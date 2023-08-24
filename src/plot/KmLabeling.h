#pragma once
#include "KpContext.h"


// ʵ�����ݱ�ע֧��

class KmLabeling
{
public:

	const KpLabel& label() const { return label_; }
	void setLabel(const KpLabel& l) {
		if (label_.align != l.align ||
			label_.spacing != l.spacing ||
			label_.font != l.font)
			labelChanged_ = true;
		label_ = l;
	}

	const KpNumericFormatter& formatter() const { return formatter_; }
	void setFormatter(const KpNumericFormatter& f) {
		if (formatter_.precision != f.precision ||
			formatter_.format != f.format)
			labelChanged_ = true;
		formatter_ = f;
	}

	bool showLabel() const { return showLabel_; }
	bool& showLabel() { return showLabel_; }

	unsigned labelingDim() const { return dimLabeling_; }
	void setLabelingDim(unsigned d) {
		dimLabeling_ = d;
		labelChanged_ = true;
	}

	bool labelChanged() const { return labelChanged_; }
	bool& labelChanged() { return labelChanged_; }

private:
	KpLabel label_;
	KpNumericFormatter formatter_;
	bool showLabel_{ false };
	unsigned dimLabeling_{ 1 }; // ʸ�����ݵĵڼ���ά����Ϊ��ע���ݣ�zero-based
	bool labelChanged_{ true };
};