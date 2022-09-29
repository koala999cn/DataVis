#include "KcActionFileDialog.h"
#include <assert.h>
#include "ImFileDialog/ImFileDialog.h"
#include "KuStrUtil.h"


KcActionFileDialog::KcActionFileDialog(KeType type, const std::string_view& title,
	const std::string_view& filter, const std::string_view& dir)
	: KvAction(title)
{
	key_ = KuStrUtil::toString(reinterpret_cast<int>(this));
	type_ = type;
	filter_ = filter;
	dir_ = dir;
}


bool KcActionFileDialog::trigger()
{
	bool res{};

	if (type_ == KeType::k_save) {
		res = ifd::FileDialog::Instance().Save(key_, name(), filter_, dir_);
	}
	else {
		res = ifd::FileDialog::Instance().Open(key_, name(), filter_, false, dir_);
	}

	if (res)
	    state_ = KeState::k_triggered;

	return res;
}


void KcActionFileDialog::update()
{
	assert(triggered());

	if (ifd::FileDialog::Instance().IsDone(key_)) {
		if (ifd::FileDialog::Instance().HasResult()) {
			result_ = ifd::FileDialog::Instance().GetResult().u8string();
			state_ = KeState::k_done;
		}
		else {
			result_.clear();
			state_ = KeState::k_cancelled;
		}

		ifd::FileDialog::Instance().Close();
	}
}
