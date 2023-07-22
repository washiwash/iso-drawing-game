#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
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

int main()
{
    RenderWindow window(VideoMode(800, 600), "Isomorphic Graph Generator", Style::Titlebar | Style::Close);
    Event ev;

    // Ask the user for the number of vertices and edges
    int numVertices, numEdges;

    while (true)
    {
        cout << "Enter the number of vertices: ";
        cin >> numVertices;
        if (numVertices < 4)
        {
            cout << "Number of vertices must be greater or equal to 4.\n";
            continue;
        }
        else
            break;
    }

    while (true)
    {
        cout << "Enter the number of edges: ";
        cin >> numEdges;
        if (numEdges < 4)
        {
            cout << "Number of edges must be greater or equal to 4.\n";
            continue;
        }
        else
            break;
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

    // Game loop
    while (window.isOpen())
    {
        // Event polling
        while (window.pollEvent(ev))
        {
            // Vertex lights up when the mouse cursor hovers over it
            if (ev.type == Event::MouseMoved && edgeToolActive && vertexCount > 0)
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
                else if (ev.key.code == Keyboard::Z && ev.key.control)
                {
                    // Undo the previous modification
                    if (!prevEdgesStack.empty() && !prevEdgeCountStack.empty())
                    {
                        // Restore the previous state
                        edges = prevEdgesStack.top();
                        edgeCount = prevEdgeCountStack.top();

                        // Pop the previous state from the stacks
                        prevEdgesStack.pop();
                        prevEdgeCountStack.pop();
                    }
                }
                break;

            case Event::MouseButtonPressed:
                if (ev.mouseButton.button == Mouse::Left)
                {
                    if (vertexToolActive && vertexCount < numVertices)
                    {
                        // Get the mouse position relative to the window
                        Vector2f mousePosition = static_cast<Vector2f>(Mouse::getPosition(window));

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

                        if (startVertexIndex == -1)
                        {
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
                                Vector2f startPoint = getCenter(vertices[startVertexIndex]);
                                Vector2f endPoint = getCenter(vertices[endVertexIndex]);

                                Vertex line[] =
                                {
                                    Vertex(startPoint, Color(50, 100, 150, 255)),
                                    Vertex(endPoint, Color(200, 150, 100, 255))
                                };

                                // Save the previous state before modifying edges
                                prevEdgesStack.push(edges);
                                prevEdgeCountStack.push(edgeCount);

                                // Add the line vertices to the vector
                                edges[edgeCount * 2] = line[0];
                                edges[edgeCount * 2 + 1] = line[1];

                                // Increment the edge count
                                edgeCount++;

                                // Reset the start vertex index
                                startVertexIndex = -1;
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
                                cout << "\nAdjacency Matrix:" << endl;

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
                                cout << "\nDegree of the Graph:" << endl;

                                for (int i = 0; i < numVertices; i++)
                                {
                                    cout << "Vertex " << i + 1 << ": " << degrees[i] << endl;
                                }

                                // Generate and display the 2-isomorphism graphs
                                vector<CircleShape> isomorphicVertices1(numVertices);
                                vector<CircleShape> isomorphicVertices2(numVertices);
                                float angleIncrement1 = 2 * 3.14159f / numVertices;
                                float angleIncrement2 = 3 * 3.14159f / numVertices;
                                float radius1 = 100.f;
                                float radius2 = 200.f;
                                Vector2f center1(300.f, 300.f);
                                Vector2f center2(600.f, 300.f);

                                for (int i = 0; i < numVertices; i++)
                                {
                                    CircleShape vertex1(10);
                                    vertex1.setFillColor(Color::Green);
                                    CircleShape vertex2(10);
                                    vertex2.setFillColor(Color::Red);

                                    float angle1 = i * angleIncrement1;
                                    float angle2 = i * angleIncrement2;
                                    Vector2f position1(center1.x + radius1 * cos(angle1), center1.y + radius1 * sin(angle1));
                                    Vector2f position2(center2.x + radius2 * cos(angle2), center2.y + radius2 * sin(angle2));

                                    vertex1.setPosition(position1);
                                    isomorphicVertices1[i] = vertex1;

                                    vertex2.setPosition(position2);
                                    isomorphicVertices2[i] = vertex2;
                                }

                                RenderWindow isomorphicWindow1(VideoMode(800, 600), "2-Isomorphism Graph 1", Style::Titlebar | Style::Close);
                                RenderWindow isomorphicWindow2(VideoMode(800, 600), "2-Isomorphism Graph 2", Style::Titlebar | Style::Close);

                                while (isomorphicWindow1.isOpen() || isomorphicWindow2.isOpen())
                                {
                                    Event isomorphicEv1, isomorphicEv2;

                                    while (isomorphicWindow1.pollEvent(isomorphicEv1))
                                    {
                                        if (isomorphicEv1.type == Event::Closed)
                                            isomorphicWindow1.close();
                                    }

                                    while (isomorphicWindow2.pollEvent(isomorphicEv2))
                                    {
                                        if (isomorphicEv2.type == Event::Closed)
                                            isomorphicWindow2.close();
                                    }

                                    isomorphicWindow1.clear(Color(0, 0, 0, 255));
                                    isomorphicWindow2.clear(Color(0, 0, 0, 255));

                                    // Draw the vertices of the isomorphic graphs
                                    for (size_t i = 0; i < isomorphicVertices1.size(); i++)
                                    {
                                        isomorphicWindow1.draw(isomorphicVertices1[i]);
                                        isomorphicWindow2.draw(isomorphicVertices2[i]);
                                    }

                                    // Draw the edges of the isomorphic graphs
                                    for (int i = 0; i < numVertices; i++)
                                    {
                                        for (int j = i + 1; j < numVertices; j++)
                                        {
                                            if (adjacencyMatrix[i][j] > 0)
                                            {
                                                Vertex line1[] =
                                                {
                                                    Vertex(isomorphicVertices1[i].getPosition(), Color(50, 100, 150, 255)),
                                                    Vertex(isomorphicVertices1[j].getPosition(), Color(50, 100, 150, 255))
                                                };
                                                Vertex line2[] =
                                                {
                                                    Vertex(isomorphicVertices2[i].getPosition(), Color(200, 150, 100, 255)),
                                                    Vertex(isomorphicVertices2[j].getPosition(), Color(200, 150, 100, 255))
                                                };

                                                isomorphicWindow1.draw(line1, 2, Lines);
                                                isomorphicWindow2.draw(line2, 2, Lines);
                                            }
                                        }
                                    }

                                    isomorphicWindow1.display();
                                    isomorphicWindow2.display();
                                }
                            }
                        }
                    }
                }
                break;
            }
        }

        // Update

        // Render
        window.clear(Color(0, 0, 0, 255)); // Clear old frame

        // Draw the vertices
        for (int i = 0; i < vertexCount; i++)
        {
            window.draw(vertices[i]);
        }

        // Draw the edges
        window.draw(&edges[0], edgeCount * 2, Lines);

        window.display(); // Tell app that window is done drawing
    }

    // End of app
    return 0;
}
