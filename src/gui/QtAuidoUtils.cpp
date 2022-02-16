#include "QtAudioUtils.h"
#include <QString>
#include <QFileDialog>
#include <QStringLiteral>
#include "KgAudioFile.h"


namespace kPrivate
{
    static QString getAudioFileFilter() 
    {
        static QString filter;
        if (!filter.isNull()) return filter;

        for (int i = 0; i < KgAudioFile::getSupportTypeCount(); i++) {
            QString x(KgAudioFile::getTypeDescription(i));
            x.append('(');
            auto exts = QString(KgAudioFile::getTypeExtension(i)).split('|');
            for (auto s : exts) {
                x.append("*.");
                x.append(s);
                x.append(' ');
            }

            x = x.trimmed();
            x.append(')');

            filter.append(x);
            if (i != KgAudioFile::getSupportTypeCount() - 1)
                filter.append(";;");
        }

        return filter;
    }
}


QString QtAudioUtils::getSavePath(QWidget* parent)
{
    auto filter = kPrivate::getAudioFileFilter();
    return QFileDialog::getSaveFileName(parent, QStringLiteral(u"保存音频"), "", filter);
}


QString QtAudioUtils::getOpenPath(QWidget* parent)
{
    auto filter = kPrivate::getAudioFileFilter();
    return QFileDialog::getOpenFileName(parent, QStringLiteral(u"打开音频"), "", filter);
}

