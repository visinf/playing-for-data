#include "EventBrowser.h"
#include "ui_EventBrowser.h"

#include <QTimer>

#include "Code/Core.h"

enum {
	COL_NAME = 0,
	COL_EID = 1,
	COL_DURATION = 2,

	COL_CURRENT,
	COL_FIND,
	COL_BOOKMARK,
};

EventBrowser::EventBrowser(Core *core, QWidget *parent) :
QFrame(parent),
ui(new Ui::EventBrowser),
m_Core(core)
{
	ui->setupUi(this);

	m_Core->AddLogViewer(this);

	ui->events->header()->resizeSection(COL_EID, 45);

	ui->events->header()->setSectionResizeMode(COL_NAME, QHeaderView::Stretch);
	ui->events->header()->setSectionResizeMode(COL_EID, QHeaderView::Interactive);
	ui->events->header()->setSectionResizeMode(COL_DURATION, QHeaderView::Interactive);

	// we set up the name column first, EID second, so that the name column gets the
	// expand/collapse widgets. Then we need to put them back in order
	ui->events->header()->moveSection(COL_NAME, COL_EID);

	// Qt doesn't allow moving the column with the expand/collapse widgets, so this
	// becomes quickly infuriating to rearrange, just disable until that can be fixed.
	ui->events->header()->setSectionsMovable(false);

	m_SizeDelegate = new SizeDelegate(QSize(0, 16));
	ui->events->setItemDelegate(m_SizeDelegate);

	m_FindHighlight = new QTimer(this);
	m_FindHighlight->setInterval(400);
	m_FindHighlight->setSingleShot(true);
	connect(m_FindHighlight, SIGNAL(timeout()), this, SLOT(on_findHighlight_timeout()));

	QObject::connect(ui->closeFind, &QToolButton::clicked, this, &EventBrowser::on_HideFindJump);
	QObject::connect(ui->closeJump, &QToolButton::clicked, this, &EventBrowser::on_HideFindJump);
	QObject::connect(ui->jumpToEID, &LineEditFocusWidget::leave, this, &EventBrowser::on_HideFindJump);
	QObject::connect(ui->findEvent, &LineEditFocusWidget::leave, this, &EventBrowser::on_HideFindJump);
	ui->jumpStrip->hide();
	ui->findStrip->hide();
	ui->bookmarkStrip->hide();
	
	m_CurrentIcon.addFile(QStringLiteral(":/Resources/flag_green.png"), QSize(), QIcon::Normal, QIcon::Off);
	m_FindIcon.addFile(QStringLiteral(":/Resources/find.png"), QSize(), QIcon::Normal, QIcon::Off);
	m_BookmarkIcon.addFile(QStringLiteral(":/Resources/asterisk_orange.png"), QSize(), QIcon::Normal, QIcon::Off);
}

EventBrowser::~EventBrowser()
{
	m_Core->RemoveLogViewer(this);
	delete ui;
	delete m_SizeDelegate;
}

void EventBrowser::OnLogfileLoaded()
{
	QTreeWidgetItem *frame = new QTreeWidgetItem((QTreeWidget *)NULL, QStringList{ QString("Frame #%1").arg(m_Core->FrameInfo()[0].frameNumber), "", "" });

	QTreeWidgetItem *framestart = new QTreeWidgetItem(frame, QStringList{ "Frame Start", "0", "" });
	framestart->setData(COL_EID, Qt::UserRole, QVariant(0));
	framestart->setData(COL_CURRENT, Qt::UserRole, QVariant(false));
	framestart->setData(COL_FIND, Qt::UserRole, QVariant(false));
	framestart->setData(COL_BOOKMARK, Qt::UserRole, QVariant(false));

	uint lastEID = AddDrawcalls(frame, m_Core->CurDrawcalls(0));
	frame->setData(COL_EID, Qt::UserRole, QVariant(lastEID));

	ui->events->insertTopLevelItem(0, frame);

	ui->events->expandItem(frame);
}

void EventBrowser::OnLogfileClosed()
{
	ui->events->clear();
}

void EventBrowser::OnEventSelected(uint32_t frameID, uint32_t eventID)
{
	SelectEvent(frameID, eventID);
}

uint EventBrowser::AddDrawcalls(QTreeWidgetItem *parent, const rdctype::array<FetchDrawcall> &draws)
{
	uint lastEID = 0;

	for(int32_t i = 0; i < draws.count; i++)
	{
		QTreeWidgetItem *child = new QTreeWidgetItem(parent, QStringList{ QString(draws[i].name.elems), QString("%1").arg(draws[i].eventID), "0.0" });
		lastEID = AddDrawcalls(child, draws[i].children);
		if(lastEID == 0) lastEID = draws[i].eventID;
		child->setData(COL_EID, Qt::UserRole, QVariant(lastEID));
		child->setData(COL_CURRENT, Qt::UserRole, QVariant(false));
		child->setData(COL_FIND, Qt::UserRole, QVariant(false));
		child->setData(COL_BOOKMARK, Qt::UserRole, QVariant(false));
	}

	return lastEID;
}

void EventBrowser::SetDrawcallTimes(QTreeWidgetItem *node, const rdctype::array<CounterResult> &results)
{
	if(node == NULL) return;

	// parent nodes take the value of the sum of their children
	double duration = 0.0;

	// look up leaf nodes in the dictionary
	if(node->childCount() == 0)
	{
		uint eid = node->data(COL_EID, Qt::UserRole).toUInt();

		duration = -1.0;

		for(int32_t i = 0; i < results.count; i++)
		{
			if(results[i].eventID == eid)
				duration = results[i].value.d;
		}

		node->setText(COL_DURATION, duration < 0.0f ? "" : QString::number(duration*1000000.0));
		node->setData(COL_DURATION, Qt::UserRole, QVariant(duration));

		return;
	}

	for(int i = 0; i < node->childCount(); i++)
	{
		SetDrawcallTimes(node->child(i), results);

		double nd = node->child(i)->data(COL_DURATION, Qt::UserRole).toDouble();

		if(nd > 0.0)
			duration += nd;
	}

	node->setText(COL_DURATION, duration < 0.0f ? "" : QString::number(duration*1000000.0));
	node->setData(COL_DURATION, Qt::UserRole, QVariant(duration));
}

void EventBrowser::on_find_clicked()
{
	ui->jumpStrip->hide();
	ui->findStrip->show();
	ui->bookmarkStrip->hide();
	ui->findEvent->setFocus();
}

void EventBrowser::on_gotoEID_clicked()
{
	ui->jumpStrip->show();
	ui->findStrip->hide();
	ui->bookmarkStrip->hide();
	ui->jumpToEID->setFocus();
}

void EventBrowser::on_toolButton_clicked()
{
	ui->jumpStrip->hide();
	ui->findStrip->hide();
	ui->bookmarkStrip->show();
}

void EventBrowser::on_timeDraws_clicked()
{
	m_Core->Renderer()->AsyncInvoke([this](IReplayRenderer *r) {

		uint32_t counters[] = { eCounter_EventGPUDuration };

		rdctype::array<CounterResult> results;
		r->FetchCounters(m_Core->CurFrame(), 0, ~0U, counters, 1, &results);

		GUIInvoke::blockcall([this, results]() {
			SetDrawcallTimes(ui->events->topLevelItem(0), results);
		});
	});
}

void EventBrowser::on_events_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
	if(previous)
	{
		previous->setData(COL_CURRENT, Qt::UserRole, QVariant(false));
		RefreshIcon(previous);
	}

	if(!current) return;

	current->setData(COL_CURRENT, Qt::UserRole, QVariant(true));
	RefreshIcon(current);

	uint EID = current->data(COL_EID, Qt::UserRole).toUInt();

	m_Core->SetEventID(this, 0, EID);
}

void EventBrowser::on_HideFindJump()
{
	ui->jumpStrip->hide();
	ui->findStrip->hide();

	ui->jumpToEID->setText("");

	ClearFindIcons();
	ui->findEvent->setText("");
	ui->findEvent->setStyleSheet("");
}

void EventBrowser::on_jumpToEID_returnPressed()
{
	bool ok = false;
	uint eid = ui->jumpToEID->text().toUInt(&ok);
	if(ok)
	{
		SelectEvent(m_Core->CurFrame(), eid);
	}
}

void EventBrowser::on_findHighlight_timeout()
{
	ClearFindIcons();

	int results = SetFindIcons(ui->findEvent->text());

	if(results > 0)
		ui->findEvent->setStyleSheet("");
	else
		ui->findEvent->setStyleSheet("QLineEdit{background-color:#ff0000;}");
}

void EventBrowser::on_findEvent_textEdited(const QString &arg1)
{
	if(arg1.isEmpty())
	{
		m_FindHighlight->stop();

		ui->findEvent->setStyleSheet("");
		ClearFindIcons();
	}
	else
	{
		m_FindHighlight->start(); // restart
	}
}


void EventBrowser::on_findEvent_returnPressed()
{
	if(m_FindHighlight->isActive())
	{
		// manually fire it instantly
		m_FindHighlight->stop();
		on_findHighlight_timeout();
	}

	if(!ui->findEvent->text().isEmpty())
	{
		Find(true);
	}
}

void EventBrowser::on_findNext_clicked()
{
	Find(true);
}

void EventBrowser::on_findPrev_clicked()
{
	Find(false);
}

void EventBrowser::RefreshIcon(QTreeWidgetItem *item)
{
	if(item->data(COL_CURRENT, Qt::UserRole).toBool())
		item->setIcon(COL_NAME, m_CurrentIcon);
	else if(item->data(COL_FIND, Qt::UserRole).toBool())
		item->setIcon(COL_NAME, m_FindIcon);
	else if(item->data(COL_BOOKMARK, Qt::UserRole).toBool())
		item->setIcon(COL_NAME, m_BookmarkIcon);
	else
		item->setIcon(COL_NAME, QIcon());
}

bool EventBrowser::FindEventNode(QTreeWidgetItem *&found, QTreeWidgetItem *parent, uint32_t frameID, uint32_t eventID)
{
	for(int i=0; i < parent->childCount(); i++)
	{
		QTreeWidgetItem *n = parent->child(i);

		uint nEID = n->data(COL_EID, Qt::UserRole).toUInt();
		uint fEID = found ? found->data(COL_EID, Qt::UserRole).toUInt() : 0;

		if(nEID >= eventID && (found == NULL || nEID <= fEID))
			found = n;

		if(nEID == eventID && n->childCount() == 0)
			return true;

		if(n->childCount() > 0)
		{
			bool exact = FindEventNode(found, n, frameID, eventID);
			if(exact) return true;
		}
	}

	return false;
}

void EventBrowser::ExpandNode(QTreeWidgetItem *node)
{
	QTreeWidgetItem *n = node;
	while(node != NULL)
	{
		node->setExpanded(true);
		node = node->parent();
	}

	if(n)
		ui->events->scrollToItem(n);
}

bool EventBrowser::SelectEvent(uint32_t frameID, uint32_t eventID)
{
	if(!m_Core->LogLoaded())
		return false;

	QTreeWidgetItem *found = NULL;
	FindEventNode(found, ui->events->topLevelItem(0), frameID, eventID);
	if(found != NULL)
	{
		ui->events->clearSelection();
		ui->events->setItemSelected(found, true);
		ui->events->setCurrentItem(found);

		ExpandNode(found);
		return true;
	}

	return false;
}

void EventBrowser::ClearFindIcons(QTreeWidgetItem *parent)
{
	for(int i=0; i < parent->childCount(); i++)
	{
		QTreeWidgetItem *n = parent->child(i);

		n->setData(COL_FIND, Qt::UserRole, QVariant(false));
		RefreshIcon(n);

		if(n->childCount() > 0)
			ClearFindIcons(n);
	}
}

void EventBrowser::ClearFindIcons()
{
	if(m_Core->LogLoaded())
		ClearFindIcons(ui->events->topLevelItem(0));
}

int EventBrowser::SetFindIcons(QTreeWidgetItem *parent, QString filter)
{
	int results = 0;
	
	for(int i=0; i < parent->childCount(); i++)
	{
		QTreeWidgetItem *n = parent->child(i);

		if(n->text(COL_NAME).contains(filter, Qt::CaseInsensitive))
		{
			n->setData(COL_FIND, Qt::UserRole, QVariant(true));
			RefreshIcon(n);
			results++;
		}

		if(n->childCount() > 0)
		{
			results += SetFindIcons(n, filter);
		}
	}

	return results;
}

int EventBrowser::SetFindIcons(QString filter)
{
	if(filter.isEmpty())
		return 0;

	return SetFindIcons(ui->events->topLevelItem(0), filter);
}

QTreeWidgetItem *EventBrowser::FindNode(QTreeWidgetItem *parent, QString filter, uint32_t after)
{
	for(int i=0; i < parent->childCount(); i++)
	{
		QTreeWidgetItem *n = parent->child(i);

		uint eid = n->data(COL_EID, Qt::UserRole).toUInt();

		if(eid > after && n->text(COL_NAME).contains(filter, Qt::CaseInsensitive))
			return n;
		
		if(n->childCount() > 0)
		{
			QTreeWidgetItem *found = FindNode(n, filter, after);

			if(found != NULL)
				return found;
		}
	}

	return NULL;
}

int EventBrowser::FindEvent(QTreeWidgetItem *parent, QString filter, uint32_t after, bool forward)
{
	if(parent == NULL) return -1;

	for(int i = forward ? 0 : parent->childCount() - 1;
			i >= 0 && i < parent->childCount();
			i += forward ? 1 : -1)
	{
		auto n = parent->child(i);

		uint eid = n->data(COL_EID, Qt::UserRole).toUInt();

		bool matchesAfter = (forward && eid > after) || (!forward && eid < after);

		if(matchesAfter)
		{
			QString name = n->text(COL_NAME);
			if(name.contains(filter, Qt::CaseInsensitive))
				return (int)eid;
		}

		if(n->childCount() > 0)
		{
			int found = FindEvent(n, filter, after, forward);

			if(found > 0)
				return found;
		}
	}

	return -1;
}

int EventBrowser::FindEvent(QString filter, uint32_t after, bool forward)
{
	if(!m_Core->LogLoaded())
		return 0;

	return FindEvent(ui->events->topLevelItem(0), filter, after, forward);
}

void EventBrowser::Find(bool forward)
{
	if(ui->findEvent->text().isEmpty())
		return;

	uint32_t curEID = m_Core->CurEvent();
	if(!ui->events->selectedItems().isEmpty())
		curEID = ui->events->selectedItems()[0]->data(COL_EID, Qt::UserRole).toUInt();

	int eid = FindEvent(ui->findEvent->text(), curEID, forward);
	if(eid >= 0)
	{
		SelectEvent(0, (uint32_t)eid);
		ui->findEvent->setStyleSheet("");
	}
	else // if(WrapSearch)
	{
		eid = FindEvent(ui->findEvent->text(), forward ? 0 : ~0U, forward);
		if(eid >= 0)
		{
			SelectEvent(0, (uint32_t)eid);
			ui->findEvent->setStyleSheet("");
		}
		else
		{
			ui->findEvent->setStyleSheet("QLineEdit{background-color:#ff0000;}");
		}
	}
}
