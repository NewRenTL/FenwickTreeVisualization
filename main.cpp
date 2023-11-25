#include <raylib.h>
#include <vector>
#include <string>
const int screenWidth = 1600;
const int screenHeight = 900;
template <class T, class K>
struct Pair
{
    T first;
    K second;
    Pair(T first, K second) : first(first), second(second){};
};

class FenwickTree
{
private:
    int n;
    std::vector<int> tree;
    std::vector<Pair<int, int>> coordenadas;
    std::vector<Pair<int, Pair<int, int>>> coordParents;
    std::vector<Pair<Pair<int, int>, Pair<int, int>>> coordLineas;
    std::vector<Pair<int, Pair<int, int>>> levels;
    // Encontrar el siguiente indice que tiene el bit menos significativo
    int FindNextIndexBITLessSignificant(int index)
    {
        return index + (index & -index);
    }
    // Encontrar el anterior indice que tiene el bit menos significativo
    int FindPrevIndexBITLessSignificant(int index)
    {
        return index - (index & -index);
    }

    int g(int i)
    {
        return i & -i;
    }

    int parentIsExists(int i)
    {
        int nx = coordParents.size();
        for (int j = 0; j < nx; j++)
        {
            if (coordParents[j].first == i)
            {
                return j;
            }
        }
        return -1;
    }

    int searchLevel(int levelY)
    {
        for (int i = 0; i < static_cast<int>(levels.size()); i++)
        {
            if (levels[i].first == levelY)
            {
                return i;
            }
        }
        return -1;
    }

public:
    FenwickTree(int size) : n(size + 1), tree(n, 0)
    {
        for (int i = 0; i < n; i++)
        {
            coordenadas.push_back(Pair(0, 0));
        }
    }

    void update(int index, int delta)
    {
        index = index + 1;
        while (index < n)
        {
            tree[index] += delta;
            index = index + g(index);
        }
    }

    int prefixSum(int index)
    {
        int sum = 0;
        index = index + 1;
        while (index > 0)
        {
            sum += tree[index];
            index = index - g(index);
        }
        return sum;
    }

    void DrawTree2()
    {
        for (int i = 0; i < n; i++)
        {
            /**/
            // Obtenemos las coordendas donde se encuentra el padre
            if (i == 0)
            {

                coordenadas[0] = Pair(screenWidth / 2, 80);
                levels.push_back(Pair(80, Pair(screenWidth / 2, 80)));
                // DrawCircle(coordenadas[0].first, coordenadas[0].second, 16, YELLOW);
                // DrawText(std::to_string(tree[i]).c_str(), coordenadas[0].first, coordenadas[0].second, 20, RED);
            }
            else
            {
                int indexParent = parentIsExists(FindPrevIndexBITLessSignificant(i)); //-1|numero
                if (indexParent == -1)
                {
                    // Aqui obtenemos las nuevas coordendas del parent
                    Pair<int, int> coordDest = coordenadas[FindPrevIndexBITLessSignificant(i)];

                    int newX = coordenadas[FindPrevIndexBITLessSignificant(i)].first - 100;
                    int newY = coordenadas[FindPrevIndexBITLessSignificant(i)].second + 200;
                    int searchLevelX = searchLevel(newY);
                    if (searchLevelX != -1)
                    {
                        // Verificamos si el X es menor o igual al maximo X de ese nivel
                        int XMaxLevel = levels[searchLevelX].second.first;
                        if (newX <= XMaxLevel)
                        {
                            // Si resulta que es menor o igual , Hallamos el valor absoluto de la diferencia
                            // Hacemos que se posiciones en nivel maximo, y le agregamos 20
                            // Esto cumpliria incluso si estan en el mismo lugar 0 + 20 , estaria posicionandolo ´+ 20 a la derecha
                            newX += abs(newX - XMaxLevel) + 60;
                            levels[searchLevelX].second.first = newX;
                        }
                    }
                    else
                    {
                        levels.push_back(Pair(newY, Pair(newX, newY)));
                    }

                    coordenadas[i] = Pair(newX, newY);
                    // Aqui colocamos la ultima posición en la que se encuentra el hijo del parent y el parent lo añadimos a la lista de coordParents
                    coordParents.push_back(Pair(FindPrevIndexBITLessSignificant(i), Pair(newX, newY)));
                    // Terminado ahora dibujamos
                    coordLineas.push_back(Pair(Pair(newX, newY), Pair(coordDest.first, coordDest.second)));
                    // DrawCircle(newX, newY, 16, YELLOW);
                    // DrawText(std::to_string(tree[i]).c_str(), newX, newY, 8, RED);
                    // DrawLine(newX, newY, coordDest.first, coordDest.second, WHITE);
                }

                if (indexParent != -1)
                {
                    Pair<int, int> coordDest = coordenadas[FindPrevIndexBITLessSignificant(i)];
                    int newX = coordParents[indexParent].second.first + 80;
                    int newY = coordParents[indexParent].second.second;
                    int searchLevelX = searchLevel(newY);
                    if (searchLevelX != -1)
                    {
                        levels[searchLevelX].second.first = newX;
                    }
                    coordenadas[i] = Pair(newX, newY);
                    coordParents[indexParent].second = Pair(newX, newY);
                    // DrawCircle(newX, newY, 16, YELLOW);
                    // DrawText(std::to_string(tree[i]).c_str(), newX, newY, 8, RED);
                    // DrawLine(newX, newY, coordDest.first, coordDest.second, WHITE);
                    coordLineas.push_back(Pair(Pair(newX, newY), Pair(coordDest.first, coordDest.second)));
                }
            }
            // Pair<int, int> coordParentX = coordenadas[FindPrevIndexBITLessSignificant(i)];
        }
    }
    void cleanDraw()
    {
    }
    void draw3()
    {

        for (int i = 0; i < static_cast<int>(coordLineas.size()); i++)
        {
            DrawLine(coordLineas[i].first.first, coordLineas[i].first.second, coordLineas[i].second.first, coordLineas[i].second.second, WHITE);
        }
        for (int i = 0; i < static_cast<int>(coordenadas.size()); i++)
        {
            DrawCircle(coordenadas[i].first, coordenadas[i].second, 20, RED);
            DrawText(std::to_string(tree[i]).c_str(), coordenadas[i].first - 4, coordenadas[i].second - 2, 18, WHITE);
            DrawText(std::to_string(i).c_str(), coordenadas[i].first - 4, coordenadas[i].second + 30, 18, YELLOW);
        }
    }
};

#define NUM_PROCESSES 5

typedef enum
{
    NONE = 0,
    COLOR_GRAYSCALE,
    COLOR_TINT,
    COLOR_INVERT,
    COLOR_CONTRAST
} ImageProcess;

static const char *processText[] = {
    "CONSTURIR BITREE",
    "BORRAR BITREE",
    "SUMA EN RANGO",
    "CAMBIAR COLOR",
    "MOSTRAR ARRAY",
};

int verificarIfExistsClick(std::vector<int> vectorVerify, int searchValue)
{
    for (int i = 0; i < static_cast<int>(vectorVerify.size()); i++)
    {
        if (searchValue == vectorVerify[i])
        {
            return i;
        }
    }
    return -1;
}
int main()
{

    // Configura la ventana de Raylib
    InitWindow(screenWidth, screenHeight, "Fenwick Tree Visualization");
    InitAudioDevice();
    Music music = LoadMusicStream("./Electroman_Adventures.mp3");

    SetTargetFPS(60);
    PlayMusicStream(music);
    int currentProcess = NONE;
    // bool textureReload = false;

    Rectangle toogleRecs[NUM_PROCESSES] = {0};
    int mouseHoverRec = -1;
    int xPosArray = 0;
    int yPosArray = 0;
    for (int i = 0; i < NUM_PROCESSES; i++)
    {
        Rectangle rectX = {40.0f, (float)(50 + 52 * i), 350, 50};
        yPosArray += (float)(50 + 52 * i);
        toogleRecs[i] = rectX;
        xPosArray = rectX.x;
        yPosArray += 50 * 2;
    }

    // Ejemplo de un Fenwick Tree representado como un array
    std::vector<int> array = {2, 1, 1, 3, 2, 3, 4, 5, 6, 7, 8, 9};
    int size = array.size();

    FenwickTree fenwickTree(size);

    // Construye el árbol Fenwick con las actualizaciones
    for (int i = 0; i < size; ++i)
    {
        fenwickTree.update(i, array[i]);
    }

    fenwickTree.DrawTree2();
    std::vector<int> ClicksX;
    // Bucle principal
    while (!WindowShouldClose())
    {
        UpdateMusicStream(music);
        for (int i = 0; i < NUM_PROCESSES; i++)
        {
            if (CheckCollisionPointRec(GetMousePosition(), toogleRecs[i]))
            {
                mouseHoverRec = i;

                if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
                {
                    currentProcess = i;
                    // textureReload = true;
                }
                int validationNoDraw = verificarIfExistsClick(ClicksX,0);
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && IsMouseButtonReleased(MOUSE_BUTTON_LEFT) == false)
                {
                    switch (i)
                    {
                    case 0:
                        if (verificarIfExistsClick(ClicksX, i) == -1)
                        {
                            ClicksX.push_back(i);
                        }
                        break;
                    case 1:
                        if(validationNoDraw != -1)
                        {
                            ClicksX.erase(ClicksX.begin() + validationNoDraw);
                        }
                        break;
                    case 2:
                        break;
                    case 3:
                        break;
                    case 4:
                        break;
                    }
                }
                break;
            }
            else
                mouseHoverRec = -1;
        }

        if (IsKeyPressed(KEY_DOWN))
        {
            currentProcess++;
            if (currentProcess > (NUM_PROCESSES - 1))
            {
                currentProcess = 0;
            }
            // textureReload = true;
        }
        else if (IsKeyPressed(KEY_UP))
        {
            currentProcess--;
            if (currentProcess < 0)
            {
                currentProcess = NUM_PROCESSES - 1;
            }
        }

        BeginDrawing();
        ClearBackground(BLACK);
        for (int i = 0; i < NUM_PROCESSES; i++)
        {
            DrawRectangleRec(toogleRecs[i], ((i == currentProcess) || (i == mouseHoverRec)) ? SKYBLUE : LIGHTGRAY);
            DrawRectangleLines((int)toogleRecs[i].x, (int)toogleRecs[i].y, (int)toogleRecs[i].width, (int)toogleRecs[i].height, ((i == currentProcess) || (i == mouseHoverRec)) ? BLUE : GRAY);
            DrawText(processText[i], (int)(toogleRecs[i].x + toogleRecs[i].width / 2 - MeasureText(processText[i], 20) / 2), (int)toogleRecs[i].y + 11, 20, ((i == currentProcess) || (i == mouseHoverRec)) ? DARKBLUE : DARKGRAY);
        }
        for (int i = 0; i < size; i++)
        {
            DrawRectangle(40.0f + i * 50, toogleRecs[NUM_PROCESSES - 1].y + toogleRecs[0].height + 40, 40, 40, BLUE);
            DrawText(std::to_string(array[i]).c_str(), 40.0f + i * 50 + 5, toogleRecs[NUM_PROCESSES - 1].y + toogleRecs[0].height + 50, 25, WHITE);
        }

        DrawText("Fenwick Tree Visualization", 20, 20, 20, WHITE);
        DrawLine(18, 42, screenWidth - 18, 42, WHITE);
        // Dibuja el Fenwick Tree en la ventana

        //fenwickTree.draw3();

        for (int i = 0; i < static_cast<int>(ClicksX.size()); i++)
        {
            switch (ClicksX[i])
            {
            case 0:
                fenwickTree.draw3();
                break;

            case 2:
                break;
            }
        }

        EndDrawing();
    }
    StopMusicStream(music);
    UnloadMusicStream(music);
    CloseAudioDevice();
    // Cierra la ventana al salir
    CloseWindow();

    return 0;
}
