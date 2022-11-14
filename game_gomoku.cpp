#include <iostream>
#include <vector>
#include <fstream>

using namespace std;

//структура игровой доски
struct board {

    static const int n = 25;            //размер доски
    static const int cnt = 5;           //кол-во элементов для победы
    static const char null_char = '.';  //нулевой символ доски
    char data[n][n];                    //представление доски матрицей символ

    //конструктор по умолчанию
    board() {
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                data[i][j] = null_char;
            }
        }
    }

    bool in_board(int i, int j) {
        return (i >= 0 && j >= 0 && i < n && j < n);
    }

    //функция проверяет доску на наличие пяти одинаковых клеток, возвращает нулевой символ или символ найденой пятерки
    char check_board() {
        for (int i = 0; i <= n-cnt; ++i) {
            for (int j = 0; j <= n-cnt; ++j) {
                if (data[i][j] == null_char) {
                    continue;
                }
                for (int dir = 0; dir < 3; ++dir) {

                    //идем от противного: считаем что нашли пять в ряд
                    bool check = true;

                    //определяем приращение по i и j по направлению
                    int dx = dir % 2 == 0;
                    int dy = dir > 0;

                    for (int k = 0; k < cnt; ++k) {
                        if (data[i + dx * k][j + dy * k] != data[i][j]) {
                            check = false; //если нашли противоречие, выходим из цикла
                            break;
                        }
                        
                    }

                    //если проверка все таки пройдена, возвращаем символ победившего игрока
                    if (check) {
                        return data[i][j];
                    }
                }
            }
        }

        //если не найдено ни одного соответствия возвращаем нулевой символ
        return null_char;
    }

    void print() {
        cout << "Игровое поле:\n\t";
        for (int j = 0; j < n; ++j) {
            printf("%c ", 'A' + j);
        }

        for (int i = 0; i < n; ++i) {
            printf("\n%2d\t", i+1);
            for (int j = 0; j < n; ++j) {
                cout << data[i][j] << ' ';
            }
            printf("\t%2d", i + 1);
        }cout << "\n\t";

        for (int j = 0; j < n; ++j) {
            printf("%c ", 'A' + j);
        }
        printf("\n\n");
    }
};

struct player {
    board* m_board;
    bool is_human;
    char m_char;

    player(board* b, bool human, char ch) {
        m_board = b;
        is_human = human;
        m_char = ch;
    }

    //функция когда игрок делает ход
    void make_move() {
        //ход для человека
        if (is_human) {
            int i, j;
            char ch;
            while (true) {
                cout << "Введите строки и столбца и строку (число и буква): ";
                cin >> i >> ch;
                --i;
                j = ch - 'A';
               
                if (i < 0 || j < 0 || i >= m_board->n || j >= m_board->n) {
                    cout << "Ошибка: выход за пределы доски.\n";
                    continue;
                }
                if (m_board->data[i][j] != m_board->null_char) {
                    cout << "Ошибка: клетка занята.\n";
                    continue;
                }
             
                m_board->data[i][j] = m_char;
                break;
            }
        }
        //ход для ИИ
        else {
            srand(time(NULL));
            for (int i = 0; i < 300000000; ++i);
            board& b = *m_board;
            int max_grade = -1;
            int max_i = 0, max_j = 0;
            int grades[b.n][b.n];
            for (int i = 0; i < b.n; ++i) {
                for (int j = 0; j < b.n; ++j) {
                    grades[i][j] = 0;
                }
            }

            for (int i = 0; i <= b.n; ++i) {
                for (int j = 0; j <= b.n; ++j) {
                    if (b.data[i][j] != b.null_char) {
                        continue;
                    }

                    int max_my_streak = 0;
                    int max_other_streak = 0;

                    //grade[i][j] оценка для клетки, чем она выше, тем больше шансов, что туда будет поставлена клетка

                    for (int dir = 0; dir < 3; ++dir) {
                        //определяем приращение по i и j по направлению
                        int dx = dir % 2 == 0;
                        int dy = dir > 0;

                        int my_streak = 0;
                        int other_streak = 0;

                        for (int k = -b.cnt+1; k < b.cnt; ++k) {
                            if (!b.in_board(i + dx * k, j + dy * k)) { continue; }
                            
                            char cur_ch = b.data[i + dx * k][j + dy * k];

                            if (cur_ch == m_char || cur_ch == b.null_char) {
                                my_streak += (cur_ch == m_char? 2 : 1);
                            }
                            else {
                                max_my_streak = max(max_my_streak, my_streak);
                                my_streak = 0;
                            }

                            if (cur_ch != m_char) {
                                other_streak += (cur_ch != b.null_char ? 2 : 1);
                            }
                            else {
                                max_other_streak = max(max_other_streak, other_streak);
                                other_streak = 0;
                            }
                        }

                        max_my_streak = max(max_my_streak, my_streak);
                        max_other_streak = max(max_other_streak, other_streak);

                        if (max_my_streak < b.cnt && max_other_streak < b.cnt) {
                            grades[i][j] = 0;
                        }
                        else {
                            grades[i][j] = max(max_other_streak, max(grades[i][j], max_my_streak));
                        }
                    }
     
                    if (grades[i][j] > max_grade) {
                        max_grade = grades[i][j];
                    }
                }
            }

            int rnd = rand() % (b.n*b.n);
            bool v = false;
            for (int i = 0; !v; i = (i+1)%b.n) {
                for (int j = 0; j < b.n; ++j) {
                    if (grades[i][j] == max_grade) {
                        if (--rnd < 0) {
                            max_i = i;
                            max_j = j;
                            v = true;
                            break;
                        }
                    }
                }
            }

            printf("Игрок выбирает клетку [%d][%c].\n", max_i+1, 'A' + max_j);
            b.data[max_i][max_j] = m_char;
        }
    }
};

struct game {
    board m_board;
    vector<player> players;

    game() {
        m_board = board();
        players.push_back(player(&m_board, false, 'X'));
        players.push_back(player(&m_board, true, 'O'));
    }

    void play() {
        m_board.print();

        for (int move = 0, pid = 0; m_board.check_board() == m_board.null_char; ++move, pid = (pid + 1) % players.size()) {
            printf("Ход игрока %d.\n", pid + 1);
            players[pid].make_move();
            cout << "Ход закончен.\n";
            m_board.print();
            char check_ch = m_board.check_board();
            if (check_ch != m_board.null_char) {
                for (size_t i = 0; i < players.size(); ++i) {
                    if (check_ch == players[i].m_char) {
                        printf("Игра закончена. Победил игрок %d.\n", i+1);
                        return;
                    }
                }
            }
        }

    }

};

int main()
{
    setlocale(LC_ALL, "Russian");

    game g;
    g.play();

    return 0;
}
 
