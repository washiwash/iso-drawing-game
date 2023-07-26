#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cmath>
#include <vector>
#include <stack>

using namespace std;
using namespace sf;

float calculateDistance(const Vector2f& point1, const Vector2f& point2)
{
    float distanceX = point2.x - point1.x;
    float distanceY = point2.y - point1.y;
    return sqrt(distanceX * distanceX + distanceY * distanceY);
}

Vector2f getCenter(const CircleShape& shape)
{
    Vector2f position = shape.getPosition();
    float radius = shape.getRadius();
    return Vector2f(position.x + radius, position.y + radius);
}

bool isInteger (string input)
{
    for (int i = 0; i < input.size(); i++)
    {
        if (!isdigit(input[i]))
            return false;
    }
    return true;
}

int main()
{
    RenderWindow window(VideoMode(1200, 600), "Isomorphic Graph Generator", Style::Titlebar | Style::Close);
    Event ev;

    // Define the designated area for drawing
    FloatRect drawingArea(0.f, 0.f, 400.f, 600.f);
    FloatRect isomorphicArea(400.f, 0.f, 800.f, 600.f);

    // Ask the user for the number of vertices and edges
    string numVert, numEdg;
    int numVertices, numEdges;

    //Sound Effects
    SoundBuffer popSound, undSound, lineSound; //load file
    popSound.loadFromFile("src/sfx/pop.wav");
    undSound.loadFromFile("src/sfx/undo.wav");
    lineSound.loadFromFile("src/sfx/line.wav");

    Sound pop, und, line;
    pop.setBuffer(popSound);
    und.setBuffer(undSound);
    line.setBuffer(lineSound);

    while (true)
    {
        cout << "Enter the number of vertices: ";
        cin >> numVert;
        if (!isInteger(numVert) || stoi(numVert) < 4)
        {
            cout << "Input must be a number greater or equal to 4.\n";
            continue;
        }
        else
        {
            numVertices = stoi(numVert);
            break;
        }
    }

    while (true)
    {
        cout << "Enter the number of edges: ";
        cin >> numEdg;
        if (!isInteger(numEdg) || stoi(numEdg) < 4)
        {
            cout << "Input must be a number greater or equal to 4.\n";
            continue;
        }
        else
        {
            numEdges = stoi(numEdg);
            break;
        }
    }

    // Create a vector to store the vertices and edges
    vector<CircleShape> vertices(numVertices);
    vector<Vertex> edges(numEdges * 2);
    stack<vector<Vertex>> prevEdgesStack;
    stack<int> prevEdgeCountStack;
    int vertexCount = 0;
    int edgeCount = 0;

    bool vertexToolActive = false;
    bool edgeToolActive = false;
    int startVertexIndex = -1;

    vector<vector<int>> adjacencyMatrix(numVertices, vector<int>(numVertices, 0));
    vector<int> degrees(numVertices, 0);

    // Create a vector to store updated degrees of vertices and type of edge
    vector<int> updatingDegree(numEdges, 0);
    vector<int> degreeIndex(numEdges);
    stack<vector<int>> prevDegreeIndexStack;
    vector<string> isLoopOrLine(numEdges);
    stack<vector<string>> prevIsLoopOrLineStack;

    // Generate and display the 2-isomorphism graphs
    vector<CircleShape> isomorphicVertices1(numVertices);
    vector<CircleShape> isomorphicVertices2(numVertices);
    float angleIncrement1 = 6.28318f / numVertices;
    float angleIncrement2 = 6.28318f / numVertices;
    float radius1 = 100.f;
    float radius2 = 100.f;

    // Calculate centers for the isomorphic graphs
    Vector2f center1(isomorphicArea.left + isomorphicArea.width / 4.f, isomorphicArea.top + isomorphicArea.height / 2.f);
    Vector2f center2(isomorphicArea.left + isomorphicArea.width * 3.f / 4.f, isomorphicArea.top + isomorphicArea.height / 2.f);

    srand(time(0));

    for (int i = 0; i < numVertices; i++)
    {
        CircleShape vertex1(5);
        vertex1.setFillColor(Color::Green);
        CircleShape vertex2(5);
        vertex2.setFillColor(Color::Red);

        float angle1 = i * angleIncrement1;
        float angle2 = i * angleIncrement2;
        Vector2f position1(center1.x + radius1 * cos(angle1), center1.y + radius1 * sin(angle1));
        Vector2f position2(center2.x + (radius2 - (-40 + (rand() % 101))) * cos(angle2), center2.y + (radius2 - (-40 + (rand() % 101))) * sin(angle2));

        vertex1.setPosition(position1);
        isomorphicVertices1[i] = vertex1;

        vertex2.setPosition(position2);
        isomorphicVertices2[i] = vertex2;
    }

    random_shuffle (isomorphicVertices1.begin(), isomorphicVertices1.end());
    random_shuffle (isomorphicVertices2.begin(), isomorphicVertices2.end());

    // Game loop
    while (window.isOpen())
    {
        // Event polling
        while (window.pollEvent(ev))
        {
            switch (ev.type)
            {
            case Event::Closed:
                window.close();
                break;

            case Event::KeyPressed:
                if (ev.key.code == Keyboard::Escape)
                    window.close();
                else if (ev.key.code == Keyboard::F)
                {
                    vertexToolActive = true;
                    edgeToolActive = false;
                    startVertexIndex = -1;
                    cout << "Vertex Tool is Active" << endl;

                }
                else if (ev.key.code == Keyboard::G)
                {
                    edgeToolActive = true;
                    vertexToolActive = false;
                    startVertexIndex = -1;
                    cout << "Edge Tool is Active" << endl;
                }
                else if (ev.key.code == Keyboard::Z && ev.key.control){
                    if(edgeCount != numEdges)
                        {
                            // Undo the previous modification
                            if (startVertexIndex == -1 && !prevEdgesStack.empty() && !prevEdgeCountStack.empty() && !prevIsLoopOrLineStack.empty() && !prevDegreeIndexStack.empty())
                            {
                                updatingDegree[degreeIndex[edgeCount]] -= 1;

                                // Restore the previous state of edges
                                edges = prevEdgesStack.top();
                                edgeCount = prevEdgeCountStack.top();    
                                isLoopOrLine = prevIsLoopOrLineStack.top();           
                                degreeIndex = prevDegreeIndexStack.top();

                                // Pop the previous state from the stacks
                                prevEdgesStack.pop();
                                prevEdgeCountStack.pop();
                                prevIsLoopOrLineStack.pop();
                                prevDegreeIndexStack.pop();

                                und.play();
                                cout << "Edge undone.\n";
                            }
                            else if (edgeCount != 0)
                                cout << "Draw the edge first before undoing an edge.\n";
                        }
                    }
                break;

            case Event::MouseButtonPressed:
                if (ev.mouseButton.button == Mouse::Left)
                {
                    if (vertexToolActive && vertexCount < numVertices)
                    {
                        // Get the mouse position relative to the window
                        pop.play();
                        Vector2f mousePosition = static_cast<Vector2f>(Mouse::getPosition(window));

                        // Check if the mouse is within the designated drawing area
                        if (!drawingArea.contains(mousePosition))
                            break;

                        // Create a circle shape at the mouse position
                        CircleShape vertex(10);
                        vertex.setFillColor(Color::White);
                        vertex.setPosition(mousePosition);

                        // Add the vertex to the vector
                        vertices[vertexCount] = vertex;

                        // Increment the vertex count
                        vertexCount++;
                    }
                    else if (edgeToolActive && edgeCount < numEdges)
                    {
                        // Get the mouse position relative to the window
                        Vector2f mousePosition = static_cast<Vector2f>(Mouse::getPosition(window));

                        if (!drawingArea.contains(mousePosition))
                            break;

                        if (startVertexIndex == -1)
                        {
                            cout << "Edge drawing tool enabled.\n";

                            // Find the closest vertex to the mouse position
                            float closestDistance = 18;
                            for (int i = 0; i < vertexCount; i++)
                            {
                                float distance = calculateDistance(mousePosition, vertices[i].getPosition());
                                if (distance < closestDistance)
                                {
                                    closestDistance = distance;
                                    startVertexIndex = i;
                                }
                            }
                        }
                        else
                        {
                            // Create an edge between the start vertex and the end vertex
                            // Find the closest vertex to the mouse position
                            float closestDistance = 18;
                            int endVertexIndex = -1;
                            for (int i = 0; i < vertexCount; i++)
                            {
                                float distance = calculateDistance(mousePosition, vertices[i].getPosition());
                                if (distance < closestDistance)
                                {
                                    closestDistance = distance;
                                    endVertexIndex = i;
                                }
                            }

                            if (endVertexIndex != -1)
                            {
                                line.play();
                                Vector2f startPoint = getCenter(vertices[startVertexIndex]);
                                Vector2f endPoint = getCenter(vertices[endVertexIndex]);

                                prevIsLoopOrLineStack.push(isLoopOrLine);
                                if (startPoint == endPoint)                                
                                    isLoopOrLine[edgeCount] = "Loop";
                                else
                                    isLoopOrLine[edgeCount] = "Line";

                                cout << isLoopOrLine[edgeCount] << endl;

                                // Save the previous state before modifying edges
                                prevEdgesStack.push(edges);
                                prevEdgeCountStack.push(edgeCount);

                                // Add the line vertices to the vector
                                edges[edgeCount * 2] = startPoint;
                                edges[edgeCount * 2 + 1] = endPoint;

                                // Increment the edge count
                                edgeCount++;

                                for (int i = 0; i < edgeCount; i++)
                                {
                                    Vector2f existingStartPoint = edges[i * 2].position;
                                    Vector2f existingEndPoint = edges[i * 2 + 1].position;
                                    if ((existingStartPoint == startPoint && existingEndPoint == endPoint) ||
                                        (existingStartPoint == endPoint && existingEndPoint == startPoint))
                                    {
                                        updatingDegree[i] += 1;
                                        prevDegreeIndexStack.push(degreeIndex);
                                        degreeIndex[edgeCount] = i;
                                        cout << "Updated degree [" << i << "] : " << updatingDegree[i] << endl;
                                        break;
                                    }
                                }
                                
                                // Reset the start vertex index
                                startVertexIndex = -1;

                                cout << "Edge drawing tool disabled.\n";
                            }

                            if (edgeCount == numEdges)
                            {
                                for (int i = 0; i < edgeCount; i++)
                                {
                                    Vector2f startPoint = edges[i * 2].position;
                                    Vector2f endPoint = edges[i * 2 + 1].position;

                                    int startVertex = -1;
                                    int endVertex = -1;

                                    // Find the index of start and end vertices in the vertices vector
                                    for (int j = 0; j < vertexCount; j++)
                                    {
                                        if (getCenter(vertices[j]) == startPoint)
                                            startVertex = j;
                                        if (getCenter(vertices[j]) == endPoint)
                                            endVertex = j;
                                    }

                                    if (startVertex != -1 && endVertex != -1 && !(startVertex == endVertex))
                                    {
                                        adjacencyMatrix[startVertex][endVertex] += 1;
                                        adjacencyMatrix[endVertex][startVertex] += 1;
                                    }
                                    else
                                        adjacencyMatrix[startVertex][endVertex] += 1;
                                }

                                // Output the adjacency matrix
                                cout << "\nAdjacency Matrix of Drawn Graph:" << endl;

                                for (int i = 0; i < numVertices; i++)
                                {
                                    for (int j = 0; j < numVertices; j++)
                                    {
                                        cout << adjacencyMatrix[i][j] << " ";
                                    }
                                    cout << endl;
                                }

                                for (int i = 0; i < numVertices; i++)
                                {
                                    int degree = 0;
                                    for (int j = 0; j < numVertices; j++)
                                    {
                                        degree += adjacencyMatrix[i][j];
                                    }
                                    degrees[i] = degree;
                                }

                                // Output the degree of each vertex
                                cout << "\nDegree of the Graph of Drawn Graph:" << endl;

                                for (int i = 0; i < numVertices; i++)
                                {
                                    cout << "Vertex " << i + 1 << ": " << degrees[i] << endl;
                                }

                                // Output the adjacency matrix for the isomorphic graph 1
                                cout << "\nAdjacency Matrix for Generated Graph 1:" << endl;
                                for (int i = 0; i < numVertices; i++)
                                {
                                    for (int j = 0; j < numVertices; j++)
                                    {
                                        int isomorphicValue = adjacencyMatrix[i][j] > 0 ? 1 : 0;
                                        cout << isomorphicValue << " ";
                                    }
                                    cout << endl;
                                }

                                for (int i = 0; i < numVertices; i++)
                                {
                                    int degree = 0;
                                    for (int j = 0; j < numVertices; j++)
                                    {
                                        degree += adjacencyMatrix[i][j] > 0 ? 1 : 0;
                                    }
                                    degrees[i] = degree;
                                }

                                // Output the degree of each vertex for the isomorphic graph 1
                                cout << "\nDegree of the Generated Graph 1:" << endl;
                                for (int i = 0; i < numVertices; i++)
                                {
                                    cout << "Vertex " << i + 1 << ": " << degrees[i] << endl;
                                }

                                // Output the adjacency matrix for the isomorphic graph 2
                                cout << "\nAdjacency Matrix for Generated Graph 2:" << endl;
                                for (int i = 0; i < numVertices; i++)
                                {
                                    for (int j = 0; j < numVertices; j++)
                                    {
                                        int isomorphicValue = adjacencyMatrix[i][j] > 0 ? 1 : 0;
                                        cout << isomorphicValue << " ";
                                    }
                                    cout << endl;
                                }

                                for (int i = 0; i < numVertices; i++)
                                {
                                    int degree = 0;
                                    for (int j = 0; j < numVertices; j++)
                                    {
                                        degree += adjacencyMatrix[i][j] > 0 ? 1 : 0;
                                    }
                                    degrees[i] = degree;
                                }

                                // Output the degree of each vertex for the isomorphic graph 2
                                cout << "\nDegree of the Generated Graph 2:" << endl;
                                for (int i = 0; i < numVertices; i++)
                                {
                                    cout << "Vertex " << i + 1 << ": " << degrees[i] << endl;
                                }
                            }
                        }
                    }
                }
                break;
            }
        }

        // Update
        // Vertex lights up when the mouse cursor hovers over it
            if (edgeToolActive && vertexCount > 0)
            {
                // Get the mouse position relative to the window
                Vector2f mousePosition = static_cast<Vector2f>(Mouse::getPosition(window));

                // Find the closest vertex to the mouse position
                float closestDistance = 18;
                for (int i = 0; i < vertexCount; i++)
                {
                    float distance = calculateDistance(mousePosition, vertices[i].getPosition());
                    if (distance < closestDistance)
                    {
                        vertices[i].setFillColor(Color::Yellow);
                    }
                    else
                    {
                        vertices[i].setFillColor(Color::White);
                    }
                }
            }
            else
            {
                for (int i = 0; i < vertexCount; i++)
                {
                    vertices[i].setFillColor(Color::White);
                }
            }

        // Render
        window.clear(Color(0, 0, 0, 255)); // Clear old frame

        // Draw the designated drawing area
        RectangleShape drawingAreaShape(Vector2f(drawingArea.width, drawingArea.height));
        drawingAreaShape.setPosition(drawingArea.left, drawingArea.top);
        drawingAreaShape.setOutlineThickness(2.f);
        drawingAreaShape.setOutlineColor(Color::White);
        drawingAreaShape.setFillColor(Color(0, 0, 0, 0));
        window.draw(drawingAreaShape);

        // Draw the vertices
        for (int i = 0; i < vertexCount; i++)
        {
            window.draw(vertices[i]);
        }

        // Draw the edges
        for (int i = 0; i < edgeCount; i++)
        {
            Vertex line[] = {edges[i * 2], edges[i * 2 + 1]};

            if (isLoopOrLine[i] == "Loop")
            {

            }  
            if (isLoopOrLine[i] == "Line")
            {
                if (updatingDegree[i] % 2 == 0)
                {

                }
                else
                    window.draw(line, 2, Lines);
            }
        }

        // Draw the isomorphic graph vertices and edges
        for (size_t i = 0; i < isomorphicVertices1.size(); i++)
        {
            window.draw(isomorphicVertices1[i]);
        }
        for (int i = 0; i < numVertices; i++)
        {
            for (int j = i + 1; j < numVertices; j++)
            {
                if (adjacencyMatrix[i][j] > 0)
                {
                    Vector2f startPoint = getCenter(isomorphicVertices1[i]);
                    Vector2f endPoint = getCenter(isomorphicVertices1[j]);

                    Vertex line1[] =
                    {
                        Vertex(startPoint, Color(50, 100, 150, 255)),
                        Vertex(endPoint, Color(50, 100, 150, 255))
                    };
                    window.draw(line1, 2, Lines);
                }
            }
        }

        for (size_t i = 0; i < isomorphicVertices2.size(); i++)
        {
            window.draw(isomorphicVertices2[i]);
        }
        for (int i = 0; i < numVertices; i++)
        {
            for (int j = i + 1; j < numVertices; j++)
            {
                if (adjacencyMatrix[i][j] > 0)
                {
                    Vector2f startPoint = getCenter(isomorphicVertices2[i]);
                    Vector2f endPoint = getCenter(isomorphicVertices2[j]);

                    Vertex line2[] =
                    {
                        Vertex(startPoint, Color(200, 150, 100, 255)),
                        Vertex(endPoint, Color(200, 150, 100, 255))
                    };
                    window.draw(line2, 2, Lines);
                }
            }
        }

        window.display(); // Tell app that window is done drawing
    }

    // End of app
    return 0;
}
