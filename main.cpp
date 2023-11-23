#include <raylib.h>
#include <vector>
#include <string>
const int screenWidth = 1920;
const int screenHeight = 1080;
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
    std::vector<Pair<int,Pair<int,int>>> levels;
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

                coordenadas[0] = Pair(screenWidth / 2, 20);
                levels.push_back(Pair(20,Pair(screenWidth/2,20)));
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
                    int newY = coordenadas[FindPrevIndexBITLessSignificant(i)].second + 150;
                    int searchLevelX = searchLevel(newY);
                    if(searchLevelX != -1)
                    {
                        //Verificamos si el X es menor o igual al maximo X de ese nivel
                        int XMaxLevel = levels[searchLevelX].second.first;
                        if(newX <= XMaxLevel)
                        {
                            //Si resulta que es menor o igual , Hallamos el valor absoluto de la diferencia
                            //Hacemos que se posiciones en nivel maximo, y le agregamos 20
                            //Esto cumpliria incluso si estan en el mismo lugar 0 + 20 , estaria posicionandolo ´+ 20 a la derecha
                            newX += abs(newX - XMaxLevel) + 20;
                            levels[searchLevelX].second.first = newX;
                        }
                    }
                    else
                    {
                        levels.push_back(Pair(newY,Pair(newX,newY)));
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
                    int newX = coordParents[indexParent].second.first + 40;
                    int newY = coordParents[indexParent].second.second;
                    int searchLevelX = searchLevel(newY);
                    if(searchLevelX != -1)
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

    void draw3()
    {
        for (int i = 0; i < static_cast<int>(coordLineas.size()); i++)
        {
            DrawLine(coordLineas[i].first.first,coordLineas[i].first.second,coordLineas[i].second.first,coordLineas[i].second.second,WHITE);
        }
        for (int i = 0; i < static_cast<int>(coordenadas.size()); i++)
        {
            DrawCircle(coordenadas[i].first, coordenadas[i].second, 8, RED);
            DrawText(std::to_string(tree[i]).c_str(),coordenadas[i].first-4,coordenadas[i].second-2,4,WHITE);
        }
    }
};

int main()
{

    // Configura la ventana de Raylib
    InitWindow(screenWidth, screenHeight, "Fenwick Tree Visualization");

    SetTargetFPS(60);

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
    // Bucle principal
    while (!WindowShouldClose())
    {

        BeginDrawing();
        ClearBackground(BLACK);
        // Dibuja el Fenwick Tree en la ventana

        fenwickTree.draw3();

        EndDrawing();
    }

    // Cierra la ventana al salir
    CloseWindow();

    return 0;
}
