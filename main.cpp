#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <cmath>

using namespace std;
using namespace sf;

float calculateDistance(const Vector2f& point1, const Vector2f& point2)
{
    float distanceX = point2.x - point1.x;
    float distanceY = point2.y - point1.y;
    return sqrt(distanceX * distanceX + distanceY * distanceY);
}

int main()
{
    RenderWindow window(VideoMode(640, 480), "Polygon Drawer", Style::Titlebar | Style::Close);
    Event ev;

    // Ask the user for the number of vertices and edges
    int numVertices, numEdges;
    cout << "Enter the number of vertices: ";
    cin >> numVertices;
    cout << "Enter the number of edges: ";
    cin >> numEdges;

    // Create a vector to store the vertices and edges
    vector<CircleShape> vertices(numVertices);
    vector<Vertex> edges(numEdges * 2);
    int vertexCount = 0;
    int edgeCount = 0;

    bool vertexToolActive = false;
    bool edgeToolActive = false;
    int startVertexIndex = -1;

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
                break;

            case Event::MouseButtonPressed:
                if (ev.mouseButton.button == Mouse::Left)
                {
                    if (vertexToolActive && vertexCount < numVertices)
                    {
                        // Get the mouse position relative to the window
                        Vector2f mousePosition = static_cast<Vector2f>(Mouse::getPosition(window));

                        // Create a circle shape at the mouse position
                        CircleShape vertex(5);
                        vertex.setFillColor(Color::Red);
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
                            float closestDistance = numeric_limits<float>::max();
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
                            float closestDistance = numeric_limits<float>::max();
                            int endVertexIndex = -1;
                            for (int i = 0; i < vertexCount; i++)
                            {
                                if (i != startVertexIndex)
                                {
                                    float distance = calculateDistance(mousePosition, vertices[i].getPosition());
                                    if (distance < closestDistance)
                                    {
                                        closestDistance = distance;
                                        endVertexIndex = i;
                                    }
                                }
                            }

                            if (endVertexIndex != -1)
                            {
                                Vector2f startPoint = vertices[startVertexIndex].getPosition();
                                Vector2f endPoint = vertices[endVertexIndex].getPosition();

                                Vertex line[] =
                                {
                                    Vertex(startPoint, Color(0,0,0,0)),
                                    Vertex(endPoint, Color(0,0,0,0))
                                };

                                // Add the line vertices to the vector
                                edges[edgeCount * 2] = line[0];
                                edges[edgeCount * 2 + 1] = line[1];

                                // Increment the edge count
                                edgeCount++;

                                // Reset the start vertex index
                                startVertexIndex = -1;
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
        for (const auto& vertex : vertices)
        {
            window.draw(vertex);
        }

        // Draw the edges
        window.draw(&edges[0], edgeCount * 2, Lines);

        window.display(); // Tell app that window is done drawing
    }

    // End of app
    return 0;
}
