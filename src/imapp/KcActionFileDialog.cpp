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


auto KcActionFileDialog::desc() const -> const std::string_view
{
	return KvAction::name();
}


bool KcActionFileDialog::trigger()
{
	bool res{};

	if (type_ == KeType::k_save) {
		res = ifd::FileDialog::Instance().Save(key_, name(), filter_, dir_);
	}
	else {
		res = ifd::FileDialog::Instance().Open(key_, name(), filter_, 
			type_ == KeType::k_open_multi, dir_);
	}

	if (res)
	    state_ = KeState::k_triggered;

	return res;
}


void KcActionFileDialog::update()
{
	assert(triggered());

	if (ifd::FileDialog::Instance().IsDone(key_)) {
		state_ = ifd::FileDialog::Instance().HasResult() ? 
			KeState::k_done : KeState::k_cancelled;

		ifd::FileDialog::Instance().Close();
	}
}


auto KcActionFileDialog::result() const -> const std::filesystem::path&
{
	return ifd::FileDialog::Instance().GetResult();
}


auto KcActionFileDialog::results() const -> const std::vector<std::filesystem::path>&
{
	return ifd::FileDialog::Instance().GetResults();
}
