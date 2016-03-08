#include "qsettingsdialog.h"
#include "qsettingsdialog_p.h"
#include <QListWidgetItem>
#include "qsettingssection.h"

#define TEST_DEFAULT(index) (d->defaultCategory ? (index + 1) : (index))

QSettingsDialog::QSettingsDialog(QObject *parent) :
	QObject(parent),
	d_ptr(new QSettingsDialogPrivate(this))
{
	this->resetCategoryIconSize();
}

QSettingsDialog::~QSettingsDialog() {}

QList<QSettingsCategory *> QSettingsDialog::categories(bool includeDefault) const
{
	const Q_D(QSettingsDialog);
	QList<QSettingsCategory *> cats = d->categories;
	if(includeDefault && d->defaultCategory)
		cats.prepend(d->defaultCategory);
	return cats;
}

int QSettingsDialog::categoryCount() const
{
	const Q_D(QSettingsDialog);
	return d->categories.size();
}

QSettingsCategory *QSettingsDialog::categoryAt(int index) const
{
	const Q_D(QSettingsDialog);
	Q_ASSERT_X2(index >= 0 && index < d->categories.size(), "index out of range");
	return d->categories.at(index);
}

int QSettingsDialog::categoryIndex(QSettingsCategory *category) const
{
	const Q_D(QSettingsDialog);
	return d->categories.indexOf(category);
}

QSettingsCategory *QSettingsDialog::insertCategory(int index, const QString &name, const QIcon &icon, const QString &toolTip)
{
	Q_D(QSettingsDialog);
	Q_ASSERT_X2(index >= 0 && index <= d->categories.size(), "index out of range");
	QSettingsCategory *cat = d->createCategory(TEST_DEFAULT(index), name, icon, toolTip);
	d->categories.insert(index, cat);

	return cat;
}

QSettingsCategory *QSettingsDialog::addCategory(const QString &name, const QIcon &icon, const QString &toolTip)
{
	const Q_D(QSettingsDialog);
	return this->insertCategory(d->categories.size(), name, icon, toolTip);
}

void QSettingsDialog::deleteCategory(int index)
{
	Q_D(QSettingsDialog);
	Q_ASSERT_X2(index >= 0 && index < d->categories.size(), "index out of range");
	d->mainDialg->deleteItem(TEST_DEFAULT(index));
	delete d->categories.takeAt(index);
}

bool QSettingsDialog::deleteCategory(QSettingsCategory *category)
{
	Q_D(QSettingsDialog);
	int index = d->categories.indexOf(category);
	if(index >= 0) {
		this->deleteCategory(index);
		return true;
	} else
		return false;
}

void QSettingsDialog::moveCategory(int from, int to)
{
	Q_D(QSettingsDialog);
	Q_ASSERT_X2(from >= 0 && from < d->categories.size(), "index out of range");
	Q_ASSERT_X2(to >= 0 && to < d->categories.size(), "index out of range");
	d->categories.move(from, to);
	d->mainDialg->moveItem(TEST_DEFAULT(from), TEST_DEFAULT(to));
}

QSettingsCategory *QSettingsDialog::defaultCategory()
{
	Q_D(QSettingsDialog);
	if(!d->defaultCategory) {
		d->defaultCategory = d->createCategory(0,
											   tr("General"),
											   QIcon(QStringLiteral(":/QSettingsDialog/icons/general.ico")));
	}
	return d->defaultCategory;
}

QSettingsSection *QSettingsDialog::defaultSection()
{
	return this->defaultCategory()->defaultSection();
}

QSettingsGroup *QSettingsDialog::defaultGroup()
{
	return this->defaultCategory()->defaultSection()->defaultGroup();
}

bool QSettingsDialog::hasDefaultCategory() const
{
	const Q_D(QSettingsDialog);
	return d->defaultCategory;
}

QSize QSettingsDialog::categoryIconSize() const
{
	const Q_D(QSettingsDialog);
	return d->mainDialg->iconSize();
}

void QSettingsDialog::showDialog()
{
	Q_D(QSettingsDialog);
	d->mainDialg->open();
}

void QSettingsDialog::setCategoryIconSize(QSize categoryIconSize)
{
	Q_D(QSettingsDialog);
	d->mainDialg->updateIconSize(categoryIconSize);
}

void QSettingsDialog::resetCategoryIconSize()
{
	Q_D(QSettingsDialog);
	int size = d->mainDialg->style()->pixelMetric(QStyle::PM_LargeIconSize);
	d->mainDialg->updateIconSize({size, size});
}



QSettingsDialogPrivate::QSettingsDialogPrivate(QSettingsDialog *q_ptr) :
	q_ptr(q_ptr),
	mainDialg(new DisplayDialog(Q_NULLPTR)),
	defaultCategory(Q_NULLPTR),
	categories()
{}

QSettingsDialogPrivate::~QSettingsDialogPrivate()
{
	delete this->defaultCategory;
	foreach (QSettingsCategory *cat, this->categories)
		delete cat;
}

QSettingsCategory *QSettingsDialogPrivate::createCategory(int index, const QString &name, const QIcon &icon, const QString &toolTip)
{
	QListWidgetItem *item = new QListWidgetItem();
	item->setText(name);
	item->setIcon(icon);
	item->setToolTip(toolTip.isEmpty() ? name : toolTip);
	QTabWidget *tab = new QTabWidget();
	tab->setTabBarAutoHide(true);

	this->mainDialg->insertItem(index, item, tab);
	return new QSettingsCategory(item, tab);
}
