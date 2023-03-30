#pragma once
#include "KpContext.h"


// 实现数据标注支持

class KmLabeling
{
public:

	const KpLabel& label() const { return label_; }
	void setLabel(const KpLabel& l) {
		if (label_.precision != l.precision ||
			label_.format != l.format ||
			label_.align != l.align ||
			label_.spacing != l.spacing)
			labelChanged_ = true;
		label_ = l;
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
	bool showLabel_{ false };
	unsigned dimLabeling_{ 1 };
	bool labelChanged_{ true };
};