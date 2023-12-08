#include <raylib.h>
#include <vector>
#include <string>
#include <cstring>
#include <time.h>

//Aqui se puede asignar a que resolución quieres trabajar la visualización

//Este es el largo del la ventana
const int screenWidth = 1900;
//Este es la altura de la ventana
const int screenHeight = 950;
//Se pueden modificar al gusto de cada quiens

//Este es mi propio Pair
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
    std::vector<int> arrayOrigin;                                  // Vector para guardar los valores array origial
    std::vector<int> tree;                                         // Vector array para formar el arbol BIT
    //Los atributos de debajo de aqui son unicamente para la visualización del arbol BIT en la interfaz grafica
    std::vector<Pair<int, int>> coordenadas;                       // Para guardar las coordenadas de los nodos
    std::vector<Pair<int, Pair<int, int>>> coordParents;           // Todos los nodos que son padres con sus coordendas ->
    std::vector<Pair<Pair<int, int>, Pair<int, int>>> coordLineas; //Vector para crear las aristas o lineas que conectan los nodos
    std::vector<Pair<int, Pair<int, int>>> levels; //Vector para calcular los niveles del arbol y asi evitar cubrimiento de otro nodo al momento de dibujar
    std::vector<Color> colors;
    std::vector<int> brands;
    std::vector<Color> colorsArrayOrigin;


    // Encontrar el siguiente indice que tiene el bit menos significativo
    int FindNextIndexBITLessSignificant(int index)
    {
        // Agarra el binario y le resta el bit menos significativo
        return index + (index & -index);
    }
    // Encontrar el anterior indice qu  e tiene el bit menos significativo
    int FindPrevIndexBITLessSignificant(int index)
    {
        return index - (index & -index);
    }

    int g(int i)
    {
        return i & -i;
    }

    //Esta función se usa principalmente para saber el nodo a donde debe conectarse por medio de una arista al nodo padre (unicamente para el proceso de dibujo)
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


    //Esta función tiene como uso principal saber en cuantos niveles el arbol ya se esta construyendo , a lo que podria llamarse altura en un arbol binario,
    //esta función es implementado tambien para evitar que los nodos ubicados en ejes X y un eje Y más profundo lleguen a solaparse, avisandome si existira
    //un nuevo nivel más abajo es decir si mis nodos anteriores se encontraban en Eje Y (250) entonces el nuevo nivel seria Eje Y (350), ayudandome de esa manera
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


    //Este es la función que se encargar de construir el BITTREE O FenwickTree
    void ConstructionBITree(std::vector<int> sourceArray)
    {   

        //Inicializamos los atributos necesarios para comenzar a dibujar nuestro bittree
        for (int i = 0; i < n; i++)
        {
            coordenadas.push_back(Pair(0, 0));
        }
        this->colors = std::vector(n, BLUE);
        this->arrayOrigin = sourceArray;
        this->colorsArrayOrigin = std::vector(arrayOrigin.size(), BLUE);
        int sizeSource = static_cast<int>(sourceArray.size());

        //Empezamos a construir nuestro BITTREE
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
        // ARRAY [0,0,0,0,0,0,0,0,0,0,0 , 0 ];
        //        0 1 2 3 4 5 6 7 8 9 10  11 ; 12 values
        // BITREE [0,0,0,0,0,0,0,0,0,0,0 , 0 , 0 ];
        //        0 1 2 3 4 5 6 7 8 9 10  11  12 ; 13 values
        tree = std::vector(n, 0);
        for (int i = 0; i < n; i++)
        {
            coordenadas.push_back(Pair(0, 0));
        }
        this->colors = std::vector(n, BLUE);
        this->colorsArrayOrigin = std::vector(arrayOrigin.size(), BLUE);
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

    //Update del fenwick Tree en tiempo log(n)
    void update(int index, int delta)
    {
        index = index + 1; // 12 , 13 , [0,[1---------]]
        while (index < n)
        {
            tree[index] += delta;
            index = index + g(index);
        }
    }

    //Suma de un elemento hasta un indice en tiempo log(n)
    int prefixSum(int index)
    {
        int sum = 0;
        index = index + 1;
        while (index > 0)
        {
            sum += tree[index];
            brands.push_back(index);
            index = index - g(index);
        }
        return sum;
    }

    bool isEmptyArrayOrigin()
    {
        return (arrayOrigin.size() == 0) ? true : false;
    }

    void push(int value)
    {
        arrayOrigin.push_back(value);
        reConstruction();
    }
    void pop()
    {
        arrayOrigin.pop_back();
        reConstruction();
    }

    std::vector<int> getTree()
    {
        return tree;
    }

    //DrawTree2 se encarga principalmente de hacer los calculos para saber a que padre debe apuntar cada nodo y al mismo tiempo donde ubicar al nodo en
    //la interfaz grafica, imaginemos que en algun momento los nodos se encuentran muy cerca y posiblemente puedan solaparse, esta función internemanete se encarga
    // de verificar que cuando esten a punto de solaparse se agreguen unos cuantos puntos al eje X para evitarlo y que se siga viendo de manera visible
    void calculando_CoordenadasDibujo()
    {
        for (int i = 0; i < n; i++)
        {
            /**/
            // Obtenemos las coordendas donde se encuentra el padre
            if (i == 0)
            {

                coordenadas[0] = Pair(screenWidth / 2, 80);
                colors[0] = RED;
                levels.push_back(Pair(80, Pair(screenWidth / 2, 80)));
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
                    colors[i] = RED;
                    // Aqui colocamos la ultima posición en la que se encuentra el hijo del parent y el parent lo añadimos a la lista de coordParents
                    coordParents.push_back(Pair(FindPrevIndexBITLessSignificant(i), Pair(newX, newY)));
                    // Pusheamos de donde a donde se creara la linea que conectara al hijo al padre
                    coordLineas.push_back(Pair(Pair(newX, newY), Pair(coordDest.first, coordDest.second)));
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
                    colors[i] = RED;
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


    //Aqui limpiamos completamente todos nuestro arbol al momento, tanto el tree principal y los otros vectores para ubicarlos nuevamente cuando se active el boton 
    //de añadir elemento
    void cleanDraw()
    {
        this->n = 0;
        tree.clear();
        coordenadas.clear();
        coordParents.clear();
        coordLineas.clear();
        colors.clear();
        levels.clear();
    }

    //Sirve unicamente para dibujar el arbol usando las coordenadas para saber donde pintar los nodos del arbol, usando posiciones , claro que tambien
    //se usan los textos para poder diferenciarlos
    void Dibujar()
    {

        for (int i = 0; i < static_cast<int>(coordLineas.size()); i++)
        {
            DrawLine(coordLineas[i].first.first, coordLineas[i].first.second, coordLineas[i].second.first, coordLineas[i].second.second, WHITE);
        }
        for (int i = 0; i < static_cast<int>(coordenadas.size()); i++)
        {
            DrawCircle(coordenadas[i].first, coordenadas[i].second, 20, colors[i]);
            DrawText(std::to_string(tree[i]).c_str(), coordenadas[i].first - 4, coordenadas[i].second - 2, 18, WHITE);
            DrawText(std::to_string(i).c_str(), coordenadas[i].first - 4, coordenadas[i].second + 30, 18, YELLOW);
        }
    }

    std::vector<int> getArrayOrigin()
    {
        return arrayOrigin;
    }
    std::vector<Color> getArrayOriginColors()
    {
        return colorsArrayOrigin;
    }

    void setColorArrayOrigin(Color color, int index)
    {
        colorsArrayOrigin[index] = color;
    }

    //Coloreamos todos los nodos que han sido usados en nuestro prefix Sum de un color AZUL
    void coloringLabels()
    {
        for (int i = 0; i < static_cast<int>(brands.size()); i++)
        {
            colors[brands[i]] = BLUE;
        }
    }

    //Limpiamos las etiquetas de colores a su color original
    // y tambien el array original lo cambiamos a su color original
    void cleanColoring()
    {
        for (int i = 0; i < static_cast<int>(brands.size()); i++)
        {
            colors[brands[i]] = RED;
        }
        brands.clear();
        colorsArrayOrigin = std::vector(arrayOrigin.size(), BLUE);
    }

    //Obtener size de fenwicktree
    int getN()
    {
        return n;
    }

    //Suma por rango usando nuestra anterior funcion PrefixSum haciendo la diferencia entre la suma hasta el indice maximo y restandole la suma hasta el indice minimo
    int range_query(int left, int right)
    {
        return prefixSum(right) - prefixSum(left - 1);
    }
};

#define NUM_PROCESSES 7

typedef enum
{
    NONE = 0,
    COLOR_GRAYSCALE,
    COLOR_TINT,
    COLOR_INVERT,
    COLOR_CONTRAST
} ImageProcess;


//Array que guarda el nombre de cada boton usando un array de cadena de caracteres
static const char *processText[] = {
    "CONSTURIR BITREE",
    "OCULTAR BITREE",
    "AGREGAR ELEMENTO",
    "SUMA INDICE RANDOM",
    "LIMPIAR ARBOL",
    "BORRAR ELEMENTO",
    "SUMA RANGO"};

//Verificar que numero de boton le ha dado click el usuario
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


#define MAX_INPUT_CHARS 4

int main()
{

    // Configura la ventana de Raylib
    InitWindow(screenWidth, screenHeight, "Fenwick Tree Visualization");
    InitAudioDevice();
    Music music = LoadMusicStream("./Electroman_Adventures.mp3");
    // std::thread musicThread(MusicThread, music);
    srand(time(nullptr));
    SetTargetFPS(60);
    PlayMusicStream(music);


    int currentProcess = NONE;

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


    //Creación de array de chars para el contenido de la caja de texto
    char inputText1[MAX_INPUT_CHARS + 1] = "\0";
    char inputText2[MAX_INPUT_CHARS + 1] = "\0";
    char inputText3[MAX_INPUT_CHARS + 1] = "\0";
    char inputText4[MAX_INPUT_CHARS + 1] = "\0";
    int value1 = -1;
    int value2 = -1;
    int value3 = -1;
    int value4 = -1;


    //Creación de Cajas de Texto
    Rectangle textBox1 = {screenWidth / 2.0f - screenWidth / 4.0f, screenHeight - 230, 100, 50};
    Rectangle textBox2 = {(float)textBox1.x + 200, screenHeight - 230, 100, 50};
    Rectangle textBox3 = {(float)toogleRecs[2].width + 50, (float)toogleRecs[2].y, 80, 50};
    Rectangle textBox4 = {(float)textBox3.x + 90, (float)textBox3.y, 80, 50};

    bool mouseOnText1 = false;
    bool mouseOnText2 = false;
    bool mouseOnText3 = false;
    bool mouseOnText4 = false;
    int framesCounter = 0;
    int framesCounter2 = 0;

    // Ejemplo de un Fenwick Tree representado como un array
    int pruebaX = 0;
    // std::vector<int> pruebas = {1, 1, 3, 2, 3, 4, 5, 6, 7, 8, 9};
    std::vector<int> pruebas = {1, 1, 3, 2, 3, 4, 5, 6, 7, 8, 9};
    std::vector<int> array = {2};         // 1 elemento
    int size = array.size();              // 1
    FenwickTree fenwickTree(size, array); // el n para el arbol por ahora


    std::vector<int> ClicksX;
    // Bucle principal
    bool IsConstruction = false;
    int indexSumX = -1;
    int randomX = -1;
    int indexSumRange = -1;
    Color colorsInputText[4] = {LIGHTGRAY, LIGHTGRAY, (Color){90, 238, 46, 255}, (Color){223, 28, 78, 255}};
    Color textPut[3] = {WHITE, WHITE, WHITE};

    while (!WindowShouldClose())
    {
        UpdateMusicStream(music);

        mouseOnText1 = CheckCollisionPointRec(GetMousePosition(), textBox1);
        mouseOnText2 = CheckCollisionPointRec(GetMousePosition(), textBox2);
        mouseOnText3 = CheckCollisionPointRec(GetMousePosition(), textBox3);
        mouseOnText4 = CheckCollisionPointRec(GetMousePosition(), textBox4);


        if (mouseOnText1 || mouseOnText2)
        {
            SetMouseCursor(MOUSE_CURSOR_IBEAM);
            char *currentInputText = (mouseOnText1) ? inputText1 : inputText2;
            // int *currentValue = (mouseOnText1) ? &value1 : &value2;
            int key = GetCharPressed();

            while (key > 0)
            {
                if ((key >= '0' && key <= '9') && (std::strlen(currentInputText) < MAX_INPUT_CHARS))
                {
                    currentInputText[std::strlen(currentInputText)] = (char)key;
                    currentInputText[std::strlen(currentInputText) + 1] = '\0';
                }

                key = GetCharPressed();
            }
            if (IsKeyPressed(KEY_BACKSPACE) && (std::strlen(currentInputText) > 0))
            {
                currentInputText[std::strlen(currentInputText) - 1] = '\0';
            }
        }
        else
        {
            SetMouseCursor(MOUSE_CURSOR_DEFAULT);
        }
        if (mouseOnText1 || mouseOnText2)
        {
            framesCounter++;
        }
        else
        {
            framesCounter = 0;
        }
        if (mouseOnText3 || mouseOnText4)
        {
            SetMouseCursor(MOUSE_CURSOR_IBEAM);
            char *currentInputText = (mouseOnText3) ? inputText3 : inputText4;
            int key = GetCharPressed();
            while (key > 0)
            {
                if ((key >= '0' && key <= '9') && (std::strlen(currentInputText) < MAX_INPUT_CHARS))
                {
                    currentInputText[std::strlen(currentInputText)] = (char)key;
                    currentInputText[std::strlen(currentInputText) + 1] = '\0';
                }
                key = GetCharPressed();
            }
            if (IsKeyPressed(KEY_BACKSPACE) && (std::strlen(currentInputText) > 0))
            {
                currentInputText[std::strlen(currentInputText) - 1] = '\0';
            }
        }

        if (mouseOnText3 || mouseOnText4)
        {
            framesCounter2++;
        }
        else
        {
            framesCounter2 = 0;
        }

        for (int i = 0; i < NUM_PROCESSES; i++)
        {
            if (CheckCollisionPointRec(GetMousePosition(), toogleRecs[i]) || (IsKeyPressed(KEY_ENTER) && (i == currentProcess)))
            {
                mouseHoverRec = i;

                int buttomPressed = -1;
                if ((IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || IsKeyPressed(KEY_ENTER)) && IsMouseButtonReleased(MOUSE_BUTTON_LEFT) == false)
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
                        buttomPressed = 3;
                        // Código para el caso 3
                    }
                    else if (i == 4)
                    {
                        buttomPressed = 4;
                        // Código para el caso 4
                    }
                    else if (i == 5)
                    {
                        buttomPressed = 5;
                        //Codigo para el caso 5
                    }
                    else if (i == 6)
                    {
                        buttomPressed = 6;
                        //Codigo para el caso 6
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
                            fenwickTree.calculando_CoordenadasDibujo();
                            // Decimos que ya está construido
                            IsConstruction = true;
                        }
                        // Se agrega recién el click
                        ClicksX.push_back(i);
                    }
                }
                else if (buttomPressed == 1)
                {
                    if (i == 1 && IsConstruction)
                    {
                        ClicksX.clear();
                    }
                }
                else if (buttomPressed == 2)
                {
                    ClicksX.clear();
                    if (IsConstruction == true)
                    {
                        if (strlen(inputText3) <= 0 && strlen(inputText4) <= 0)
                        {
                            if (pruebaX <= static_cast<int>(pruebas.size()) - 1)
                            {
                                //
                                fenwickTree.cleanDraw();
                                fenwickTree.push(pruebas[pruebaX]); // Push and Reconstrucion
                                pruebaX++;
                                fenwickTree.calculando_CoordenadasDibujo();
                                ClicksX.push_back(i);
                            }
                            else
                            {
                                // Limpieza
                                fenwickTree.cleanDraw();
                                fenwickTree.push(1 + rand() % 9 + 1 - 1);
                                fenwickTree.calculando_CoordenadasDibujo();
                                ClicksX.push_back(i);
                            }
                        }
                        else
                        {
                            if (strlen(inputText3) > 0 && strlen(inputText4) > 0)
                            {

                                colorsInputText[2] = (Color){231, 17, 17, 255};
                                colorsInputText[3] = (Color){231, 17, 17, 255};
                            }
                            else if (strlen(inputText3) > 0 && strlen(inputText4) <= 0)
                            {
                                colorsInputText[2] = (Color){90, 238, 46, 255};
                                colorsInputText[3] = (Color){223, 28, 78, 255};
                                value3 = atoi(inputText3);
                                fenwickTree.cleanDraw();
                                fenwickTree.push(value3);
                                fenwickTree.calculando_CoordenadasDibujo();
                                ClicksX.push_back(i);
                            }
                            else if (strlen(inputText4) > 0 && strlen(inputText3) <= 0)
                            {
                                colorsInputText[2] = (Color){90, 238, 46, 255};
                                colorsInputText[3] = (Color){223, 28, 78, 255};
                                value4 = atoi(inputText4);
                                fenwickTree.cleanDraw();
                                fenwickTree.push(value4-2*value4);
                                fenwickTree.calculando_CoordenadasDibujo();
                                ClicksX.push_back(i);
                            }
                        }
                    }
                    else
                    {
                        if (strlen(inputText3) <= 0)
                        {
                            if (pruebaX <= static_cast<int>(pruebas.size()) - 1)
                            {
                                //
                                fenwickTree.cleanDraw();
                                fenwickTree.push(pruebas[pruebaX]); // Push and Reconstrucion
                                pruebaX++;
                                fenwickTree.calculando_CoordenadasDibujo();
                                IsConstruction = true;
                                ClicksX.push_back(i);
                            }
                            else
                            {
                                // Limpieza
                                fenwickTree.cleanDraw();
                                fenwickTree.push(1 + rand() % 9 + 1 - 1);
                                fenwickTree.calculando_CoordenadasDibujo();
                                IsConstruction = true;
                                ClicksX.push_back(i);
                            }
                        }
                        else
                        {
                            if (strlen(inputText3) > 0 && strlen(inputText4) > 0)
                            {
                                colorsInputText[2] = (Color){231, 17, 17, 255};
                                colorsInputText[3] = (Color){231, 17, 17, 255};
                            }
                            else if (strlen(inputText3) > 0 && strlen(inputText4) <= 0)
                            {
                                colorsInputText[2] = (Color){90, 238, 46, 255};
                                colorsInputText[3] = (Color){223, 28, 78, 255};
                                value3 = atoi(inputText3);
                                fenwickTree.cleanDraw();
                                fenwickTree.push(value3 - 2*value3);
                                fenwickTree.calculando_CoordenadasDibujo();
                                IsConstruction = true;
                                ClicksX.push_back(i);
                            }
                            else if (strlen(inputText4) > 0 && strlen(inputText3) <= 0)
                            {
                                colorsInputText[2] = (Color){90, 238, 46, 255};
                                colorsInputText[3] = (Color){223, 28, 78, 255};
                                value4 = atoi(inputText4);
                                fenwickTree.cleanDraw();
                                fenwickTree.push(value4 - 2*value4);
                                fenwickTree.calculando_CoordenadasDibujo();
                                IsConstruction = true;
                                ClicksX.push_back(i);
                            }
                        }
                    }
                }
                else if (buttomPressed == 3)
                {
                    if (IsConstruction == true && verificarIfExistsClick(ClicksX, 3) == -1)
                    {
                        // Selecciono un indice que se encuentre en dentro del BITree del fenwick
                        randomX = rand() % static_cast<int>(fenwickTree.getArrayOrigin().size());
                        // Hallo la suma
                        indexSumX = fenwickTree.prefixSum(randomX);
                        // Coloreo las etiquetas durante el recorrido del prefixSum
                        fenwickTree.coloringLabels();
                        ClicksX.push_back(i);
                    }
                }
                else if (buttomPressed == 4)
                { // Verifico si hay colores en azul
                    int verifyColorOnBlue = verificarIfExistsClick(ClicksX, 3);
                    int verifyColorOnBlueRange = verificarIfExistsClick(ClicksX, 6);
                    // Si resulta que se ha presionado el boton 3;
                    if (IsConstruction == true && (verifyColorOnBlue != -1 || verifyColorOnBlueRange != -1))
                    {
                        fenwickTree.cleanColoring();
                        if (verifyColorOnBlue != -1)
                        {
                            ClicksX.erase(ClicksX.begin() + verifyColorOnBlue);
                        }
                        if (verifyColorOnBlueRange != -1)
                        {
                            ClicksX.erase(ClicksX.begin() + verifyColorOnBlueRange);
                        }
                        ClicksX.push_back(i);
                    }
                    textPut[0] = WHITE;
                    textPut[1] = WHITE;
                    textPut[2] = WHITE;
                    indexSumX = -1;
                    randomX = -1;
                    indexSumRange = -1;
                    value1 = -1;
                    value2 = -1;
                }
                else if (buttomPressed == 5)
                {
                    ClicksX.clear();
                    if (IsConstruction && fenwickTree.isEmptyArrayOrigin() == false)
                    {
                        fenwickTree.cleanDraw();
                        fenwickTree.pop();
                        fenwickTree.calculando_CoordenadasDibujo();
                        ClicksX.push_back(i);
                    }
                }
                else if (buttomPressed == 6)
                {
                    if (strlen(inputText1) > 0 && strlen(inputText2) > 0 && IsConstruction)
                    {
                        value1 = atoi(inputText1);
                        value2 = atoi(inputText2);
                        if (value1 < value2 && value2 < static_cast<int>(fenwickTree.getArrayOrigin().size()) && value1 < static_cast<int>(fenwickTree.getArrayOrigin().size()))
                        {
                            indexSumRange = fenwickTree.range_query(value1, value2);
                            colorsInputText[0] = LIGHTGRAY;
                            colorsInputText[1] = LIGHTGRAY;
                            fenwickTree.coloringLabels();
                            ClicksX.push_back(i);
                            textPut[0] = GREEN;
                            textPut[1] = GREEN;
                            textPut[2] = GREEN;
                        }
                        else
                        {
                            if (value1 > value2 || value1 >= static_cast<int>(fenwickTree.getArrayOrigin().size()))
                            {
                                colorsInputText[0] = RED;
                            }
                            if (value2 >= static_cast<int>(fenwickTree.getArrayOrigin().size()) || value2 < value1)
                            {
                                colorsInputText[1] = RED;
                            }
                        }
                    }
                }

                if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) || (IsKeyPressed(KEY_ENTER)))
                {
                    currentProcess = i;
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
        }
        else if (IsKeyPressed(KEY_UP))
        {
            currentProcess--;
            if (currentProcess < 0)
            {
                currentProcess = NUM_PROCESSES - 1;
            }
        }

        // Todo esto se dibuja en un fotograma
        BeginDrawing();
        ClearBackground(BLACK);

        //Dibujo los botones
        for (int i = 0; i < NUM_PROCESSES; i++)
        {
            DrawRectangleRec(toogleRecs[i], ((i == currentProcess) || (i == mouseHoverRec)) ? SKYBLUE : LIGHTGRAY);
            DrawRectangleLines((int)toogleRecs[i].x, (int)toogleRecs[i].y, (int)toogleRecs[i].width, (int)toogleRecs[i].height, ((i == currentProcess) || (i == mouseHoverRec)) ? BLUE : GRAY);
            DrawText(processText[i], (int)(toogleRecs[i].x + toogleRecs[i].width / 2 - MeasureText(processText[i], 20) / 2), (int)toogleRecs[i].y + 11, 20, ((i == currentProcess) || (i == mouseHoverRec)) ? DARKBLUE : DARKGRAY);
        }
        /*Evaluando*/
        DrawRectangleRec(textBox3, colorsInputText[2]);
        if (mouseOnText3)
        {
            DrawRectangleLines((int)textBox3.x, (int)textBox3.y, (int)textBox3.width, (int)textBox3.height, RED);
        }
        else
        {
            DrawRectangleLines((int)textBox3.x, (int)textBox3.y, (int)textBox3.width, (int)textBox3.height, DARKGRAY);
        }
        DrawText(inputText3, (int)textBox3.x + 5, (int)textBox3.y + 8, ((int)textBox3.height) / 2 + 5, MAROON);

        DrawRectangleRec(textBox4, colorsInputText[3]);
        if (mouseOnText4)
        {
            DrawRectangleLines((int)textBox4.x, (int)textBox4.y, (int)textBox4.width, (int)textBox4.height, RED);
        }
        else
        {
            DrawRectangleLines((int)textBox4.x, (int)textBox4.y, (int)textBox4.width, (int)textBox4.height, DARKGRAY);
        }


        DrawText(inputText4, (int)textBox4.x + 5, (int)textBox4.y + 8, ((int)textBox4.height) / 2 + 5, WHITE);

        DrawRectangleRec(textBox1, colorsInputText[0]);
        if (mouseOnText1)
        {
            DrawRectangleLines((int)textBox1.x, (int)textBox1.y, (int)textBox1.width, (int)textBox1.height, RED);
        }
        else
        {
            DrawRectangleLines((int)textBox1.x, (int)textBox1.y, (int)textBox1.width, (int)textBox1.height, DARKGRAY);
        }
        DrawText("Min :", (int)textBox1.x - 80, (int)textBox1.y + 8, ((int)textBox1.height) / 2 + 5, WHITE);
        DrawText(inputText1, (int)textBox1.x + 5, (int)textBox1.y + 8, ((int)textBox1.height) / 2 + 5, MAROON);
        DrawRectangleRec(textBox2, colorsInputText[1]);
        if (mouseOnText2)
        {
            DrawRectangleLines((int)textBox2.x, (int)textBox2.y, (int)textBox2.width, (int)textBox2.height, RED);
        }
        else
        {
            DrawRectangleLines((int)textBox2.x, (int)textBox2.y, (int)textBox2.width, (int)textBox2.height, DARKGRAY);
        }
        DrawText("Max :", (int)textBox2.x - 80, (int)textBox2.y + 8, ((int)textBox2.height) / 2 + 5, WHITE);
        DrawText(inputText2, (int)textBox2.x + 5, (int)textBox2.y + 8, ((int)textBox2.height) / 2 + 5, MAROON);

        //
        std::string textilSum = "Suma Indice : " + std::to_string((indexSumX != -1) ? indexSumX : -1);
        DrawText(textilSum.c_str(), toogleRecs[NUM_PROCESSES - 1].x + toogleRecs[NUM_PROCESSES - 1].width / 4, toogleRecs[NUM_PROCESSES - 1].y + toogleRecs[0].height * 2, 30, WHITE);
        std::string randomSum = "Random Indice : " + std::to_string((randomX != -1) ? randomX : -1);
        DrawText(randomSum.c_str(), toogleRecs[NUM_PROCESSES - 1].x + toogleRecs[NUM_PROCESSES - 1].width / 4, toogleRecs[NUM_PROCESSES - 1].y + toogleRecs[0].height * 3, 30, WHITE);

        std::string sumrange = "SumRange : " + std::to_string((indexSumRange != -1) ? indexSumRange : -1);
        DrawText(sumrange.c_str(), toogleRecs[NUM_PROCESSES - 1].x + toogleRecs[NUM_PROCESSES - 1].width / 4, toogleRecs[NUM_PROCESSES - 1].y + toogleRecs[0].height * 4, 30, textPut[0]);
        std::string value1X = "Value 1 Min : " + std::to_string((value1 != -1) ? value1 : -1);
        DrawText(value1X.c_str(), toogleRecs[NUM_PROCESSES - 1].x + toogleRecs[NUM_PROCESSES - 1].width / 4, toogleRecs[NUM_PROCESSES - 1].y + toogleRecs[0].height * 5, 30, textPut[1]);
        std::string value2X = "Value 2 Max: " + std::to_string((value2 != -1) ? value2 : -1);
        DrawText(value2X.c_str(), toogleRecs[NUM_PROCESSES - 1].x + toogleRecs[NUM_PROCESSES - 1].width / 4, toogleRecs[NUM_PROCESSES - 1].y + toogleRecs[0].height * 6, 30, textPut[2]);

        // Draw array tree fenwick
        for (int i = 0; i < static_cast<int>(fenwickTree.getTree().size()); i++)
        {
            DrawText(std::to_string(i).c_str(),55.0f + i * 50,screenHeight - 150,22,PURPLE);
            DrawRectangle(40.0f + i * 50, screenHeight - 130, 40, 40, ORANGE);
            DrawText(std::to_string(fenwickTree.getTree()[i]).c_str(), 40.0f + i * 50 + 5, screenHeight - 130, 25, WHITE);
        }
        // Draw array origin
        for (int i = 0; i < static_cast<int>(fenwickTree.getArrayOrigin().size()); i++)
        {
            // toogleRecs[NUM_PROCESSES - 1].y + toogleRecs[0].height + 40
            // toogleRecs[NUM_PROCESSES - 1].y + toogleRecs[0].height + 50
            int tamanioArrayOriginFw = static_cast<int>(fenwickTree.getArrayOrigin().size());
            if (value1 != -1 && value2 != -1 && value1 < tamanioArrayOriginFw && value2 < tamanioArrayOriginFw)
            {
                if (i >= value1 && i <= value2)
                {
                    DrawRectangle(40.0f + i * 50, screenHeight - 70, 40, 40, GREEN);
                    DrawText(std::to_string(fenwickTree.getArrayOrigin()[i]).c_str(), 40.0f + i * 50 + 5, screenHeight - 70, 25, WHITE);
                }
                else
                {
                    DrawRectangle(40.0f + i * 50, screenHeight - 70, 40, 40, BLUE);
                    DrawText(std::to_string(fenwickTree.getArrayOrigin()[i]).c_str(), 40.0f + i * 50 + 5, screenHeight - 70, 25, WHITE);
                }
            }
            else if (randomX != -1)
            {
                if (i <= randomX)
                {
                    DrawRectangle(40.0f + i * 50, screenHeight - 70, 40, 40, GREEN);
                    DrawText(std::to_string(fenwickTree.getArrayOrigin()[i]).c_str(), 40.0f + i * 50 + 5, screenHeight - 70, 25, WHITE);
                }
                else
                {
                    DrawRectangle(40.0f + i * 50, screenHeight - 70, 40, 40, BLUE);
                    DrawText(std::to_string(fenwickTree.getArrayOrigin()[i]).c_str(), 40.0f + i * 50 + 5, screenHeight - 70, 25, WHITE);
                }
            }
            else
            {
                DrawRectangle(40.0f + i * 50, screenHeight - 70, 40, 40, BLUE);
                DrawText(std::to_string(fenwickTree.getArrayOrigin()[i]).c_str(), 40.0f + i * 50 + 5, screenHeight - 70, 25, WHITE);
            }
        }

        DrawText("Fenwick Tree Visualization", 20, 20, 25, WHITE);
        DrawLine(18, 42, screenWidth - 18, 42, WHITE);
        // Dibuja el Fenwick Tree en la ventana
        for (int i = 0; i < static_cast<int>(ClicksX.size()); i++)
        {
            switch (ClicksX[i])
            {
            case 0:
                if (IsConstruction == true)
                {
                    fenwickTree.Dibujar();
                }
                break;
            // El caso 1 ya no es necesario debido que por defecto lo va a ocultar
            case 2:
                // Dibujo el arbol ya reconstruido con el nuevo elemento
                fenwickTree.Dibujar();
                break;
            case 3:
                fenwickTree.Dibujar(); 
                break;
            case 4:
                fenwickTree.Dibujar();
                break;
            case 5:
                fenwickTree.Dibujar();
                break;
            case 6:
                fenwickTree.Dibujar();
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
