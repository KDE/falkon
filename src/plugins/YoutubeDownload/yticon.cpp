#include "yticon.h"

YtIcon::YtIcon(QObject *parent)
	: AbstractButtonInterface(parent)
{
	setIcon(QIcon::fromTheme(QSL("im-youtube"), QIcon(QSL(":ytdownload/data/icon.svg"))));
	setTitle(tr("Download video"));
	setToolTip(tr("Download video"));

	connect(this, &AbstractButtonInterface::clicked, this, &YtIcon::clicked);

	updateState();
}

QString YtIcon::id() const
{
	return QSL("youtube-download");
}

QString YtIcon::name() const
{
	return tr("Download Youtube");
}

void YtIcon::updateState()
{
/*    setVisible(m_manager->downloadsCount() > 0);
	const int count = m_manager->activeDownloadsCount();
	if (count > 0) {
		setBadgeText(QString::number(count));
	} else {
		setBadgeText(QString());
	}*/
}

void YtIcon::clicked(ClickController *controller)
{
	Q_UNUSED(controller)

	//mApp->downloadManager()->show();
}
