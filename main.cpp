#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include <iostream>
#include <vector>

//Global variables
static sf::Vector2i WindowSize;
static float Gravity;
static std::vector<sf::Sprite*> DrawableCollection;

class Platform : public sf::Sprite{
    std::pair<float,float> HorizontalColisionBorder;
    std::pair<float,float> VerticalColisionBorder;

public:
    Platform(){}
    Platform(sf::Texture &Texture,sf::IntRect Paramiters):Sprite(Texture,Paramiters){}

    virtual ~Platform() {}

    std::pair<float,float>& GetHorizontalColisionBorder(){
        SetHorizontalColisionBorder();
        return HorizontalColisionBorder;
    }

    void SetHorizontalColisionBorder(){
        float LeftBorder = getGlobalBounds().left;
        float RightBorder = getGlobalBounds().left + getGlobalBounds().width;
        HorizontalColisionBorder = std::pair<float,float>(LeftBorder,RightBorder);
    }

    std::pair<float,float>& GetVerticalColisionBorder(){
        SetVerticalColisionBorder();
        return VerticalColisionBorder;
    }

    void SetVerticalColisionBorder(){
        float TopBorder = getGlobalBounds().top;
        float BottomBorder = getGlobalBounds().top + getGlobalBounds().height;
        VerticalColisionBorder = std::pair<float,float>(TopBorder,BottomBorder);
    }
};

class AnimatedSprite : public sf::Sprite{
    std::vector<sf::IntRect> Frames;
    int CurrentFrame = 0;
    float AnimationFPS = 4.f;
    sf::Time CurrentFrameTime;

    int UserMovmentSpeed;
    std::pair<float,float> HorizontalColisionBorder;
    std::pair<float,float> VerticalColisionBorder;

    bool TouchingGround;
    float UpwardsSpeed;
public:
    AnimatedSprite(const sf::Texture &Texture) : sf::Sprite(Texture){}

    int& GetUserMovmentSpeed(){return UserMovmentSpeed;}
    void SetUserMovmentSpeed(int NewUserMovmentSpeed){UserMovmentSpeed = NewUserMovmentSpeed;}

    std::pair<float,float>& GetHorizontalColisionBorder(){
        SetHorizontalColisionBorder();
        return HorizontalColisionBorder;
    }

    void SetHorizontalColisionBorder(){
        float LeftBorder = getGlobalBounds().left;
        float RightBorder = getGlobalBounds().left + getGlobalBounds().width;
        HorizontalColisionBorder = std::pair<float,float>(LeftBorder,RightBorder);
    }

    std::pair<float,float>& GetVerticalColisionBorder(){
        SetVerticalColisionBorder();
        return VerticalColisionBorder;
    }

    void SetVerticalColisionBorder(){
        float TopBorder = getGlobalBounds().top;
        float BottomBorder = getGlobalBounds().top + getGlobalBounds().height;
        VerticalColisionBorder = std::pair<float,float>(TopBorder,BottomBorder);
    }

    bool& GetTouchingGround(){return TouchingGround;}
    void SetTouchingGround(bool NewTouchingGround){TouchingGround = NewTouchingGround;}

    float& GetUpwardsSpeed(){return UpwardsSpeed;}
    void SetUpwardsSpeed(float NewUpwardsSpeed){UpwardsSpeed = NewUpwardsSpeed;}

    void AddAnimationFrame(const sf::IntRect &Frame){
        if(Frames.empty()){
            setTextureRect(Frame);
        }
        Frames.push_back(Frame);
    }

    bool CheckForAllColisions(std::string Direction){
        sf::Rect<float> Intersection;
        for(auto Object : DrawableCollection){
            if(dynamic_cast<Platform*>(Object)){
                if(getGlobalBounds().intersects(Object->getGlobalBounds(),Intersection))
                {
                    if(Direction == "Bottom"){
                        move(0,-0.5);
                        TouchingGround = true;
                        return true;
                    }
                }
            }
        }

        return false;
    }

    void Step(sf::Time TimeElapsed){
        CurrentFrameTime += TimeElapsed;
        if(CurrentFrameTime.asSeconds() >= 1.f/AnimationFPS){
            setTextureRect(Frames[CurrentFrame]);
            CurrentFrame = (CurrentFrame + 1) % Frames.size();
            CurrentFrameTime = CurrentFrameTime.Zero;
        }

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::A) && GetHorizontalColisionBorder().first > 0){
            move((GetUserMovmentSpeed() * (-1) * TimeElapsed.asSeconds()),0);
        }else if(sf::Keyboard::isKeyPressed(sf::Keyboard::D) && GetHorizontalColisionBorder().second < WindowSize.x){
            move(GetUserMovmentSpeed() * TimeElapsed.asSeconds(),0);
        }

        if(GetVerticalColisionBorder().second >= WindowSize.y){
            setPosition(getPosition().x,WindowSize.y - 80);
            TouchingGround = true;
        }

        if(!TouchingGround && !CheckForAllColisions("Bottom")){
            move(0,UpwardsSpeed * TimeElapsed.asSeconds());
            UpwardsSpeed += Gravity;
        }

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && TouchingGround){
            SetUpwardsSpeed(-250);
            TouchingGround = false;
        }
    }
};

int main()
{
    srand(time(NULL));

    WindowSize = sf::Vector2i(800,600);
    std::vector<sf::Texture> TextureCollection;

    Gravity = 5;

    sf::RenderWindow Window(sf::VideoMode(static_cast <unsigned int>(WindowSize.x), static_cast <unsigned int>(WindowSize.y)), "My window");
    Window.setFramerateLimit(60);
    Window.setVerticalSyncEnabled(true);

    sf::Event Event;
    sf::Clock GameClock;
    sf::Time TimeElapsed;

    sf::Texture Texture;

    //Loading textures
    if(Texture.loadFromFile("Images\\Character\\character.png")){
        TextureCollection.push_back(Texture);
    }

    if(Texture.loadFromFile("Images\\Map\\tileset.png")){
        TextureCollection.push_back(Texture);
    }

    //Creating Character
    AnimatedSprite CharacterSprite(TextureCollection[0]);

    CharacterSprite.scale(2,2);

    for(int i = 0 ; i < 3 ; i++){
        CharacterSprite.AddAnimationFrame(sf::IntRect(i * 50 ,0,50,37));
    }

    CharacterSprite.SetUserMovmentSpeed(150);
    CharacterSprite.SetUpwardsSpeed(0);
    CharacterSprite.SetTouchingGround(false);

    DrawableCollection.push_back(&CharacterSprite);

    //Creating Platforms
    for(int i = 0 ; i < 6 ; i++){
        DrawableCollection.push_back(new Platform(TextureCollection[1],sf::IntRect(190,190,50,50)));
    }

    DrawableCollection[1]->setPosition(0,600);
    DrawableCollection[2]->setPosition(150,500);
    DrawableCollection[3]->setPosition(300,400);
    DrawableCollection[4]->setPosition(450,300);
    DrawableCollection[5]->setPosition(600,200);
    DrawableCollection[6]->setPosition(750,100);

    while (Window.isOpen()) {
        while (Window.pollEvent(Event)) {
            if (Event.type == sf::Event::Closed){
                Window.close();
            }
        }

        Window.clear(sf::Color::Black);
        for (auto it = DrawableCollection.begin(); it != DrawableCollection.end(); ++it){
            Window.draw(**it);
        }


        TimeElapsed = GameClock.getElapsedTime();

        CharacterSprite.Step(TimeElapsed);

        GameClock.restart();
        Window.display();
    }
    return 0;
}
