#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void reload();
    void format(QLineEdit *le);
    int row(QString query);

private slots:
    void on_act_new_triggered();

    void on_act_edit_triggered();

    void on_act_delete_triggered();

    void on_pb_cancel_clicked();

    void on_pb_save_clicked();

    void on_pb_date_clicked();

    void on_tableView_clicked(const QModelIndex &index);

    void on_calendarWidget_clicked(const QDate &date);

    void on_le_exit_textChanged();

    void on_le_login_textChanged();

    void on_le_total_textChanged();

    void on_act_allDelete_triggered();

    void on_act_inExcel_triggered();

    void on_act_ofExcel_triggered();

    void on_act_about_triggered();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
