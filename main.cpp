#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <cmath>

enum Algorithm
{
    None = -1,
    BubbleSort = 0,
    InsertionSort = 1,
    SelectionSort = 2
};

sf::SoundBuffer generateTone(float frequency, int durationMs, unsigned sampleRate = 44100)
{
    int sampleCount = durationMs * sampleRate / 1000;
    std::vector<sf::Int16> samples(sampleCount);
    for (int i = 0; i < sampleCount; ++i)
    {
        float t = i / static_cast<float>(sampleRate);
        samples[i] = static_cast<sf::Int16>(32767 * sin(2 * M_PI * frequency * t));
    }
    sf::SoundBuffer buffer;
    buffer.loadFromSamples(samples.data(), sampleCount, 1, sampleRate);
    return buffer;
}

int main()
{
    const int width = 3500, height = 1800, size = 100;
    std::vector<int> array(size);
    srand(static_cast<unsigned>(time(0)));

    sf::RenderWindow window(sf::VideoMode(width, height), "Sorting Visualization");
    window.setFramerateLimit(60);

    sf::Font font;
    if (!font.loadFromFile("arial.ttf"))
        return -1;
    sf::Text dropdownText;
    dropdownText.setFont(font);
    dropdownText.setCharacterSize(32); // Increased text size
    dropdownText.setFillColor(sf::Color::Black);

    std::vector<std::string> algNames = {"Bubble Sort", "Insertion Sort", "Selection Sort"};
    int selectedAlg = None;
    bool dropdownOpen = false;
    sf::FloatRect dropdownRect(10, 10, 400, 60); // Increased button size
    bool started = false;

    int bI = 0, bJ = 0;
    int insI = 1, insJ = 1;
    int selI = 0, selJ = 1, selMin = 0;
    bool sortingDone = false;

    static sf::SoundBuffer toneBuffer;
    sf::Sound sound;
    float barWidth = static_cast<float>(width) / size;

    auto reset = [&]()
    {
        for (int k = 0; k < size; k++)
            array[k] = rand() % height;
        bI = bJ = 0;
        insI = insJ = 1;
        selI = 0;
        selJ = 1;
        selMin = 0;
        sortingDone = false;
    };

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
            {
                float mx = event.mouseButton.x, my = event.mouseButton.y;
                if (dropdownRect.contains(mx, my))
                {
                    dropdownOpen = !dropdownOpen;
                }
                else if (dropdownOpen)
                {
                    for (int idx = 0; idx < static_cast<int>(algNames.size()); idx++)
                    {
                        sf::FloatRect itemRect(10, 10 + 60 * (idx + 1), 400, 60); // Adjusted item size
                        if (itemRect.contains(mx, my))
                        {
                            selectedAlg = idx;
                            dropdownOpen = false;
                            reset();
                            started = true;
                        }
                    }
                }
            }
        }

        if (started && !sortingDone && selectedAlg != None)
        {
            switch (selectedAlg)
            {
            case BubbleSort:
                if (bI < size - 1)
                {
                    if (bJ < size - bI - 1)
                    {
                        if (array[bJ] > array[bJ + 1])
                        {
                            std::swap(array[bJ], array[bJ + 1]);
                            float freq = 200.0f + (array[bJ] / static_cast<float>(height)) * 1000.0f;
                            toneBuffer = generateTone(freq, 30);
                            sound.setBuffer(toneBuffer);
                            sound.play();
                        }
                        bJ++;
                    }
                    else
                    {
                        bJ = 0;
                        bI++;
                    }
                }
                else
                    sortingDone = true;
                break;
            case InsertionSort:
                if (insI < size)
                {
                    if (insJ > 0 && array[insJ - 1] > array[insJ])
                    {
                        std::swap(array[insJ - 1], array[insJ]);
                        float freq = 200.0f + (array[insJ] / static_cast<float>(height)) * 1000.0f;
                        toneBuffer = generateTone(freq, 30);
                        sound.setBuffer(toneBuffer);
                        sound.play();
                        insJ--;
                    }
                    else
                    {
                        insI++;
                        insJ = insI;
                    }
                }
                else
                    sortingDone = true;
                break;
            case SelectionSort:
                if (selI < size - 1)
                {
                    if (selJ < size)
                    {
                        float freqcmp = 200.0f + (array[selJ] / static_cast<float>(height)) * 1000.0f;
                        toneBuffer = generateTone(freqcmp, 15);
                        sound.setBuffer(toneBuffer);
                        sound.play();
                        if (array[selJ] < array[selMin])
                            selMin = selJ;
                        selJ++;
                    }
                    else
                    {
                        if (selMin != selI)
                        {
                            std::swap(array[selI], array[selMin]);
                            float freq = 200.0f + (array[selI] / static_cast<float>(height)) * 1000.0f;
                            toneBuffer = generateTone(freq, 30);
                            sound.setBuffer(toneBuffer);
                            sound.play();
                        }
                        selI++;
                        selMin = selI;
                        selJ = selI + 1;
                    }
                }
                else
                    sortingDone = true;
                break;
            }
        }

        window.clear();

        for (int k = 0; k < size; k++)
        {
            sf::RectangleShape bar(sf::Vector2f(barWidth - 1, array[k]));
            bool highlight = false;
            if (!sortingDone && started && selectedAlg != None)
            {
                if (selectedAlg == BubbleSort && (k == bJ || k == bJ + 1))
                    highlight = true;
                else if (selectedAlg == InsertionSort && (k == insJ || k == insJ - 1))
                    highlight = true;
                else if (selectedAlg == SelectionSort && (k == selMin || k == selJ - 1))
                    highlight = true;
            }
            bar.setFillColor(highlight ? sf::Color::Red : sf::Color::White);
            bar.setPosition(k * barWidth, height - array[k]);
            window.draw(bar);
        }

        // Draw dropdown
        sf::RectangleShape drShape(sf::Vector2f(dropdownRect.width, dropdownRect.height));
        drShape.setPosition(dropdownRect.left, dropdownRect.top);
        drShape.setFillColor(sf::Color::White);
        drShape.setOutlineColor(sf::Color::Black);
        drShape.setOutlineThickness(2);
        window.draw(drShape);

        dropdownText.setString(selectedAlg == None ? "Select Algorithm" : algNames[selectedAlg]);
        dropdownText.setPosition(dropdownRect.left + 15, dropdownRect.top + 10);
        window.draw(dropdownText);

        if (dropdownOpen)
        {
            for (int idx = 0; idx < static_cast<int>(algNames.size()); idx++)
            {
                sf::FloatRect itemRect(10, 10 + 60 * (idx + 1), 400, 60);
                sf::RectangleShape itShape(sf::Vector2f(itemRect.width, itemRect.height));
                itShape.setPosition(itemRect.left, itemRect.top);
                itShape.setFillColor(sf::Color::White);
                itShape.setOutlineColor(sf::Color::Black);
                itShape.setOutlineThickness(2);
                window.draw(itShape);

                dropdownText.setString(algNames[idx]);
                dropdownText.setPosition(itemRect.left + 15, itemRect.top + 10);
                window.draw(dropdownText);
            }
        }

        window.display();
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }

    return 0;
}
