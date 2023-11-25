#include <raylib.h>
#include <vector>
#include <string>
#include <time.h>
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
    std::vector<int> arrayOrigin;
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

    void ConstructionBITree(std::vector<int> sourceArray)
    {
        for (int i = 0; i < n; i++)
        {
            coordenadas.push_back(Pair(0, 0));
        }
        this->arrayOrigin = sourceArray;
        int sizeSource = static_cast<int>(sourceArray.size());
        for (int i = 0; i < sizeSource; i++)
        {
            update(i, sourceArray[i]);
        }
    }

    // Solo se usa para cuando se hace el push y ya el array esta dentro de la clase;
    // Y si usas esto significaria que tu ya has limpiado todos los demas vectores excepto el array original
    void reConstruction()
    {

        n = static_cast<int>(this->arrayOrigin.size()) + 1;
        tree = std::vector(n, 0);
        for (int i = 0; i < n; i++)
        {
            coordenadas.push_back(Pair(0, 0));
        }
        int sizeSource = static_cast<int>(this->arrayOrigin.size());
        for (int i = 0; i < sizeSource; i++)
        {
            update(i, this->arrayOrigin[i]);
        }
    }

public:
    FenwickTree(int size) : n(size + 1), tree(n, 0)
    {
        for (int i = 0; i < n; i++)
        {
            coordenadas.push_back(Pair(0, 0));
        }
    }

    FenwickTree(int size, std::vector<int> &sourceArray) : n(size + 1), tree(n, 0)
    {
        ConstructionBITree(sourceArray);
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

    void push(int value)
    {
        arrayOrigin.push_back(value);
        reConstruction();
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
        this->n = 0;
        tree.clear();
        coordenadas.clear();
        coordParents.clear();
        coordLineas.clear();
        levels.clear();
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

    std::vector<int> getArrayOrigin()
    {
        return arrayOrigin;
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
    "AGREGAR ELEMENTO",
    "SUMA EN RANGO",
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
    srand(time(NULL));
    SetTargetFPS(60);
    PlayMusicStream(music);
    int currentProcess = NONE;
    // bool textureReload = false;

    Rectangle toogleRecs[NUM_PROCESSES] = {0};
    int mouseHoverRec = -1;
    // int xPosArray = 0;
    int yPosArray = 0;
    for (int i = 0; i < NUM_PROCESSES; i++)
    {
        Rectangle rectX = {40.0f, (float)(50 + 52 * i), 350, 50};
        yPosArray += (float)(50 + 52 * i);
        toogleRecs[i] = rectX;
        // xPosArray = rectX.x;
        yPosArray += 50 * 2;
    }

    // Ejemplo de un Fenwick Tree representado como un array
    int pruebaX = 0;
    std::vector<int> pruebas = {1, 1, 3, 2, 3, 4, 5, 6, 7, 8, 9};
    std::vector<int> array = {2};
    int size = array.size();
    FenwickTree fenwickTree(size, array);

    // Construye el árbol Fenwick con las actualizaciones
    /*
    for (int i = 0; i < size; ++i)
    {
        fenwickTree.update(i, array[i]);
    }
    */

    // fenwickTree.DrawTree2();
    std::vector<int> ClicksX;
    // Bucle principal
    bool IsConstruction = false;
    while (!WindowShouldClose())
    {
        UpdateMusicStream(music);
        for (int i = 0; i < NUM_PROCESSES; i++)
        {
            if (CheckCollisionPointRec(GetMousePosition(), toogleRecs[i]))
            {
                mouseHoverRec = i;

                int buttomPressed = -1;
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && IsMouseButtonReleased(MOUSE_BUTTON_LEFT) == false)
                {
                    if (i == 0)
                    {
                        buttomPressed = 0;
                    }
                    else if (i == 1)
                    {
                        buttomPressed = 1;
                    }
                    else if (i == 2)
                    {
                        // Código para el caso 2
                        buttomPressed = 2;
                    }
                    else if (i == 3)
                    {
                        // Código para el caso 3
                    }
                    else if (i == 4)
                    {
                        // Código para el caso 4
                    }
                }

                if (buttomPressed == 0)
                {
                    if (i == 0 && verificarIfExistsClick(ClicksX, i) == -1)
                    {
                        // Una vez se ha verificado que el click no está

                        // Revisamos si el árbol está construido
                        // Si no está construido, lo construimos
                        if (!IsConstruction)
                        {
                            // Construimos el árbol BIT
                            fenwickTree.DrawTree2();
                            // Decimos que ya está construido
                            IsConstruction = true;
                        }
                        // Se agrega recién el click
                        ClicksX.push_back(i);
                    }
                }
                else if (buttomPressed == 1)
                {
                    int validationNoDraw = verificarIfExistsClick(ClicksX, 0);
                    if (i == 1 && validationNoDraw != -1 && IsConstruction)
                    {
                        ClicksX.erase(ClicksX.begin() + validationNoDraw);
                    }
                }
                else if (buttomPressed == 2)
                {
                    // Limpio el arbol de coordenadas,dibujos y demas
                    if (IsConstruction == true)
                    {

                        // No dibujo el arbol vacio porque va a ser tan rapido que no se va a ver
                        // Añado un nuevo elemento al array principal y reconstruyo
                        if (pruebaX <= static_cast<int>(pruebas.size() - 1))
                        {
                            fenwickTree.cleanDraw();
                            fenwickTree.push(pruebas[pruebaX]);
                            pruebaX++;
                            fenwickTree.DrawTree2();
                        }
                        else
                        {
                            fenwickTree.cleanDraw();
                            fenwickTree.push(1 + rand() % (3 + 1 - 1));
                            pruebaX++;
                            fenwickTree.DrawTree2();
                        }
                        // Y listo ya tengo listo mi nuevo arbol con el elemento agregado
                        // Solo queda que cuando llegue a dibujarlo
                        
                    }
                    else
                    {
                        fenwickTree.cleanDraw();
                        fenwickTree.push(1 + rand() % (3 + 1 - 1));
                        // Si en caso aun no se habia construido el arbol
                        // Colocamos que ya se construyo
                        fenwickTree.DrawTree2();
                        IsConstruction = true;
                    }
                }

                if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
                {
                    currentProcess = i;
                    // textureReload = true;
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
        for (int i = 0; i < static_cast<int>(fenwickTree.getArrayOrigin().size()); i++)
        {
            DrawRectangle(40.0f + i * 50, toogleRecs[NUM_PROCESSES - 1].y + toogleRecs[0].height + 40, 40, 40, BLUE);
            DrawText(std::to_string(fenwickTree.getArrayOrigin()[i]).c_str(), 40.0f + i * 50 + 5, toogleRecs[NUM_PROCESSES - 1].y + toogleRecs[0].height + 50, 25, WHITE);
        }

        DrawText("Fenwick Tree Visualization", 20, 20, 20, WHITE);
        DrawLine(18, 42, screenWidth - 18, 42, WHITE);
        // Dibuja el Fenwick Tree en la ventana

        // fenwickTree.draw3();

        for (int i = 0; i < static_cast<int>(ClicksX.size()); i++)
        {
            switch (ClicksX[i])
            {
            case 0:
                if (IsConstruction == true)
                {
                    fenwickTree.draw3();
                }
                break;
            // El caso 1 ya no es necesario ya que por defecto lo va a ocultar
            case 2:
                // Dibujo el arbol ya reconstruido con el nuevo elemento
                fenwickTree.draw3();
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
