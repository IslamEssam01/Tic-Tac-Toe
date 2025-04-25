#ifndef BACKGROUND_WIDGET_H
#define BACKGROUND_WIDGET_H

#include <QWidget>
#include <QPainter>
#include <QColor>
#include <QLinearGradient>

class BackgroundWidget : public QWidget {
    Q_OBJECT
public:
    enum class Mode { TicTacToeBoard, RepeatingPattern };
    
    explicit BackgroundWidget(Mode mode, QWidget *parent = nullptr)
        : QWidget(parent), m_mode(mode) {}
    
protected:
    void paintEvent(QPaintEvent *event) override {
        QPainter painter(this);
        
        // Draw the gradient background (light purple to dark purple)
        QLinearGradient gradient(0, 0, 0, height());
        gradient.setColorAt(0, QColor(90, 50, 150));  // Lighter purple at the top
        gradient.setColorAt(1, QColor(50, 30, 100));  // Darker purple at the bottom
        painter.fillRect(rect(), gradient);

        // Draw subtle diagonal lines for the grid-like pattern
        painter.setPen(QPen(QColor(255, 255, 255, 30), 1));  // Semi-transparent white
        int step = 40;  // Spacing between lines
        for (int i = -height(); i < width() + height(); i += step) {
            painter.drawLine(i, 0, i + height(), height());  // Diagonal from top-left to bottom-right
            painter.drawLine(i, height(), i + height(), 0);  // Diagonal from bottom-left to top-right
        }

        if (m_mode == Mode::TicTacToeBoard) {
            // Draw Xs and Os with gradients and shadows, positioned like the image
            QFont font("Arial", 40, QFont::Bold);
            painter.setFont(font);

            // X gradient (pink to purple)
            QLinearGradient xGradient(0, 0, 50, 50);
            xGradient.setColorAt(0, QColor(255, 105, 180));  // Pink
            xGradient.setColorAt(1, QColor(147, 112, 219));  // Purple

            // O gradient (yellow to orange)
            QLinearGradient oGradient(0, 0, 50, 50);
            oGradient.setColorAt(0, QColor(255, 215, 0));  // Yellow
            oGradient.setColorAt(1, QColor(255, 165, 0));  // Orange

            // Draw shadows for 3D effect
            painter.setPen(Qt::NoPen);

            // Top-left O (like the image)
            painter.setBrush(QBrush(oGradient));
            painter.drawEllipse(30, 30, 50, 50);  // Main O
            painter.setBrush(QColor(0, 0, 0, 50));  // Shadow
            painter.drawEllipse(33, 33, 50, 50);  // Offset shadow

            // Top-right X
            painter.setBrush(QBrush(xGradient));
            painter.drawText(width() - 80, 30, 50, 50, Qt::AlignCenter, "X");
            painter.setBrush(QColor(0, 0, 0, 50));
            painter.drawText(width() - 77, 33, 50, 50, Qt::AlignCenter, "X");

            // Bottom-left X
            painter.setBrush(QBrush(xGradient));
            painter.drawText(30, height() - 80, 50, 50, Qt::AlignCenter, "X");
            painter.setBrush(QColor(0, 0, 0, 50));
            painter.drawText(33, height() - 77, 50, 50, Qt::AlignCenter, "X");

            // Bottom-right O
            painter.setBrush(QBrush(oGradient));
            painter.drawEllipse(width() - 80, height() - 80, 50, 50);
            painter.setBrush(QColor(0, 0, 0, 50));
            painter.drawEllipse(width() - 77, height() - 77, 50, 50);
        } else if (m_mode == Mode::RepeatingPattern) {
            // Draw repeating Xs and Os with gradients and shadows
            QFont font("Arial", 40, QFont::Bold);
            painter.setFont(font);

            // X gradient (pink to purple)
            QLinearGradient xGradient(0, 0, 50, 50);
            xGradient.setColorAt(0, QColor(255, 105, 180));  // Pink
            xGradient.setColorAt(1, QColor(147, 112, 219));  // Purple

            // O gradient (yellow to orange)
            QLinearGradient oGradient(0, 0, 50, 50);
            oGradient.setColorAt(0, QColor(255, 215, 0));  // Yellow
            oGradient.setColorAt(1, QColor(255, 165, 0));  // Orange

            painter.setPen(Qt::NoPen);
            int gridSize = 100;  // Spacing between symbols
            for (int x = 0; x < width(); x += gridSize) {
                for (int y = 0; y < height(); y += gridSize) {
                    if ((x / gridSize + y / gridSize) % 2 == 0) {
                        // Draw X
                        painter.setBrush(QBrush(xGradient));
                        painter.drawText(x, y, gridSize, gridSize, Qt::AlignCenter, "X");
                        painter.setBrush(QColor(0, 0, 0, 50));
                        painter.drawText(x + 3, y + 3, gridSize, gridSize, Qt::AlignCenter, "X");
                    } else {
                        // Draw O
                        painter.setBrush(QBrush(oGradient));
                        painter.drawEllipse(x + gridSize/4, y + gridSize/4, gridSize/2, gridSize/2);
                        painter.setBrush(QColor(0, 0, 0, 50));
                        painter.drawEllipse(x + gridSize/4 + 3, y + gridSize/4 + 3, gridSize/2, gridSize/2);
                    }
                }
            }
        }
    }

private:
    Mode m_mode;
};

#endif