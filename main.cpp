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

int main()
{
    RenderWindow window(VideoMode(640, 480), "Polygon Drawer", Style::Titlebar | Style::Close);
    Event ev;

    // Ask the user for the number of vertices and edges
    int numVertices = 0, numEdges = 0;

    // Import font file
    Font roboto;
    if (!roboto.loadFromFile("src/font/Roboto-Light.ttf"))
    {
        cerr << "Failed to load font file" << endl;
        return 1;
    }

    sf::Text promptText;
    promptText.setFont(roboto);
    promptText.setCharacterSize(24);
    promptText.setFillColor(sf::Color::White);
    promptText.setPosition(20, 20);
    
    sf::Text inputText;
    inputText.setFont(roboto);
    inputText.setCharacterSize(24);
    inputText.setFillColor(sf::Color::White);
    inputText.setPosition(20, 60);

    bool isEnteringVertices = true;
    bool isEnteringEdges = false;

    // Create a vector to store the vertices and edges
    vector<CircleShape> vertices;
    vector<Vertex> edges;
    stack<vector<Vertex>> prevEdgesStack;
    stack<int> prevEdgeCountStack;
    int vertexCount = 0;
    int edgeCount = 0;

    bool vertexToolActive = false;
    bool edgeToolActive = false;
    int startVertexIndex = -1;

    while (true)
    {
        // Event polling
        while (window.pollEvent(ev))
        {
            switch (ev.type)
            {
            case Event::Closed:
                window.close();
                break;

            case Event::TextEntered:
                if (isEnteringVertices)
                {
                    if (ev.text.unicode >= '0' && ev.text.unicode <= '9')
                    {
                        numVertices = ev.text.unicode - '0';

                        inputText.setString(to_string(numVertices));
                    }
                    else if (ev.text.unicode == '\r')
                    {
                        isEnteringVertices = false;
                        isEnteringEdges = true;

                        promptText.setString("Enter the number of edges: ");
                        inputText.setString("");
                    }
                }
                else if (isEnteringEdges)
                {
                    if (ev.text.unicode >= '0' && ev.text.unicode <= '9')
                    {
                        numEdges = ev.text.unicode - '0';

                        inputText.setString(to_string(numEdges));
                    }
                    else if (ev.text.unicode == '\r')
                    {
                        isEnteringEdges = false;
                        // Continue with the rest of the code
                        break;
                    }
                }
                break;
            }
        }

        if (isEnteringVertices)
        {
            promptText.setString("Enter the number of vertices: ");
        }
        else if (isEnteringEdges)
        {
            promptText.setString("Enter the number of edges: ");
        }
        else
        {
            if (numVertices < 4)
            {
                promptText.setString("Number of vertices must be greater or equal to 4. Re-enter the number of vertices: ");
                isEnteringVertices = true;
                inputText.setString("");
                continue;
            }

            if (numEdges < 4)
            {
                promptText.setString("Number of edges must be greater or equal to 4. Re-enter the number of edges: ");
                isEnteringEdges = true;
                inputText.setString("");
                continue;
            }

            // Handle other events and update the game state
            switch (ev.type)
            {
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
                            CircleShape vertex(5);
                            vertex.setFillColor(Color::White);
                            vertex.setPosition(mousePosition);

                            // Add the vertex to the vector
                            vertices.push_back(vertex);

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
                                        Vertex(startPoint, Color(50, 100, 150, 255)),
                                        Vertex(endPoint, Color(200, 150, 100, 255))
                                    };

                                    // Save the previous state before modifying edges
                                    prevEdgesStack.push(edges);
                                    prevEdgeCountStack.push(edgeCount);

                                    // Add the line vertices to the vector
                                    edges.push_back(line[0]);
                                    edges.push_back(line[1]);

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

        // Render
        window.clear(Color::Black);

        // Draw the vertices
        for (int i = 0; i < vertexCount; i++)
        {
            window.draw(vertices[i]);
        }

        window.draw(promptText);
        window.draw(inputText);

        // Draw the edges
        window.draw(&edges[0], edgeCount * 2, Lines);

        window.display();
    }

    // End of app
    return 0;
}
