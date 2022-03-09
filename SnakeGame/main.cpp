/*

  Set up to use the SFML 2.5.1 64-bit API
    SFML documentation: https://www.sfml-dev.org/learn.php
*/

#include <iostream>
#include <cmath>
#include <vector>
#include <utility>
#include <algorithm>
// SFML header file for graphics, there are also ones for Audio, Window, System and Network
#include <SFML/Graphics.hpp>
#include<SFML/Window/Mouse.hpp>



struct Ball
{
    float Pos_X, Pos_Y;
    float Origin_X, Origin_Y;
    float Velocity_X= 0, Velocity_Y = 0;
    float Acceleration_X = 0, Acceleration_Y = 0;
    float Radius;
    float Mass = 0;
    float TimeRemaining;
    std::string Name;
    sf::Color Color;
};

struct Line
{
    int StartX, StartY;
    int EndX, EndY;
    std::string Name;
    sf::Vertex Drawable[2] =
    {
        sf::Vertex(sf::Vector2f(0, 0)), // top left
        sf::Vertex(sf::Vector2f(0, 0)) // top right
    };
};

bool BallsOverlapTest(float PosX_1, float PosY_1, float Radius_1, float PosX_2, float PosY_2, float Radius_2)
{
   bool Test = fabs((PosX_1 - PosX_2) * (PosX_1 - PosX_2) + (PosY_1 - PosY_2) * (PosY_1 - PosY_2)) <= (Radius_1 + Radius_2) * (Radius_1 + Radius_2);
   return Test;
}

void StaticColison(Ball* Ball_1, Ball* Ball_2)
{
    //Pythagorus to work out the distance that the balls overlap
    float OverlapDist = sqrtf((Ball_1->Pos_X - Ball_2->Pos_X) * (Ball_1->Pos_X - Ball_2->Pos_X) + (Ball_1->Pos_Y - Ball_2->Pos_Y) * (Ball_1->Pos_Y - Ball_2->Pos_Y));

    float Overlap = 0.5 * (OverlapDist - Ball_1->Radius - Ball_2->Radius);
    //Displace Ball 1
    Ball_1->Pos_X += Overlap * (Ball_1->Pos_X - Ball_2->Pos_X) / OverlapDist;
    Ball_1->Pos_Y += Overlap * (Ball_1->Pos_Y - Ball_2->Pos_Y) / OverlapDist;
    //Displace Ball 2
    Ball_2->Pos_X -= Overlap * (Ball_2->Pos_X - Ball_1->Pos_X) / OverlapDist;
    Ball_2->Pos_Y -= Overlap * (Ball_2->Pos_Y - Ball_1->Pos_Y) / OverlapDist;

}

void EdgeColision(Ball* Ball_1, Ball* Ball_2)
{
    //Pythagorus to work out the distance that the balls overlap
    float OverlapDist = sqrtf((Ball_1->Pos_X - Ball_2->Pos_X) * (Ball_1->Pos_X - Ball_2->Pos_X) + (Ball_1->Pos_Y - Ball_2->Pos_Y) * (Ball_1->Pos_Y - Ball_2->Pos_Y));

    float Overlap = 0.5 * (OverlapDist - Ball_1->Radius - Ball_2->Radius);

    Ball_1->Velocity_X -= Ball_2->Velocity_X;
    Ball_1->Velocity_Y -= Ball_2->Velocity_Y;
   

}

void MoivingColison(Ball* Ball_1, Ball* Ball_2)
{
    //Pythagorus to work out the distance that the balls overlap
    float OverlapDist = sqrtf((Ball_1->Pos_X - Ball_2->Pos_X) * (Ball_1->Pos_X - Ball_2->Pos_X) + (Ball_1->Pos_Y - Ball_2->Pos_Y) * (Ball_1->Pos_Y - Ball_2->Pos_Y));

    //Normal Vector Between Circles
    float NormalX = (Ball_2->Pos_X - Ball_1->Pos_X) / OverlapDist;
    float NormalY = (Ball_2->Pos_Y - Ball_1->Pos_Y) / OverlapDist;

    //Tangent Vectort
    float TangentX = -NormalY;
    float TangentY =  NormalX;

    //Tangental respose (Dot Product of the tranget)
    float Dot_Tangental_Ball_1 = (Ball_1->Velocity_X * TangentX) + (Ball_1->Velocity_Y * TangentY);
    float Dot_Tangental_Ball_2 = (Ball_2->Velocity_X * TangentX) + (Ball_2->Velocity_Y * TangentY);


    //Normal respose (Dot Product of the Normal)
    float Dot_Normal_Ball_1 = (Ball_1->Velocity_X * NormalX) + (Ball_1->Velocity_Y * NormalY);
    float Dot_Normal_Ball_2 = (Ball_2->Velocity_X * NormalX) + (Ball_2->Velocity_Y * NormalY);

    // Conservation of Momentum
    float Ball_1_Momentum = (Dot_Normal_Ball_1 * (Ball_1->Mass - Ball_2->Mass) + 2.0f * Ball_2->Mass * Dot_Normal_Ball_2) / (Ball_1->Mass + Ball_2->Mass);
    float Ball_2_Momentum = (Dot_Normal_Ball_2 * (Ball_2->Mass - Ball_1->Mass) + 2.0f * Ball_1->Mass * Dot_Normal_Ball_1) / (Ball_2->Mass + Ball_1->Mass);

    // Apply Tangental and Normal Resposne
    Ball_1->Velocity_X = TangentX * Dot_Tangental_Ball_1 + NormalX * Ball_1_Momentum;
    Ball_1->Velocity_Y = TangentY * Dot_Tangental_Ball_1 + NormalY * Ball_1_Momentum;

    Ball_2->Velocity_X = (TangentX * Dot_Tangental_Ball_2) + (NormalX * Ball_2_Momentum);
    Ball_2->Velocity_Y = (TangentY * Dot_Tangental_Ball_2) + (NormalY * Ball_2_Momentum);

}


void Balls_Update(std::vector <Ball*> BallList, sf::Time Passed, int ScreenWidth, int ScreenHeight)
{
    for (Ball* Ball : BallList)
    {
        Ball->Acceleration_X = -Ball->Velocity_X * 1.0f;
        Ball->Acceleration_Y = -Ball->Velocity_Y * 1.0f;
            
        Ball->Velocity_X += Ball->Acceleration_X * Passed.asSeconds();
        Ball->Velocity_Y += Ball->Acceleration_Y * Passed.asSeconds();

        Ball->Pos_X += Ball->Velocity_X * Passed.asSeconds();
        Ball->Pos_Y += Ball->Velocity_Y * Passed.asSeconds();
        
        if (fabs(Ball->Velocity_X * Ball->Velocity_X + Ball->Velocity_Y * Ball->Velocity_Y) < 0.1f)
        {
            Ball->Velocity_X = 0;
            Ball->Velocity_Y = 0;
        }

    }

}


int main()
{
 // Config
    int ScreenWidth = 800;
    int ScreenHeight = 600;
    int NumberOfBalls = 5;
    sf::RenderWindow window(sf::VideoMode(ScreenWidth, ScreenHeight), "Real Time Sim");
    float Speed = 0.25;
    int UpdateCount = 4; //How many times the Pysics updates are run per frame
    //Setup Balls
    std::vector <Ball*> BallList;

   


    Ball PlayerBall;
    PlayerBall.Pos_X = 300;
    PlayerBall.Pos_Y = 300;
    PlayerBall.Radius = 20;
    PlayerBall.Mass = PlayerBall.Radius * 5;
    PlayerBall.Name = "Player";
    PlayerBall.Color = sf::Color::Red;

    BallList.push_back(&PlayerBall);

    Ball OtherBall_1;
    OtherBall_1.Pos_X = 200;
    OtherBall_1.Pos_Y = 300;
    OtherBall_1.Radius = 20;
    OtherBall_1.Mass = OtherBall_1.Radius * 5;
    OtherBall_1.Name = "1";
    OtherBall_1.Color = sf::Color::Blue;

    BallList.push_back(&OtherBall_1);
    
    Ball OtherBall_2;
    OtherBall_2.Pos_X = 400;
    OtherBall_2.Pos_Y = 200;
    OtherBall_2.Radius = 20;
    OtherBall_2.Mass = OtherBall_2.Radius * 5;
    OtherBall_2.Name = "2";
    OtherBall_2.Color = sf::Color::Green;

    BallList.push_back(&OtherBall_2);


    Ball* SelectedBall = nullptr;

    

    //Setup Table
    std::vector<Line*> Table;

    //Table Top
    Line GameTable_Top;
    GameTable_Top.StartX = 150;
    GameTable_Top.StartY = 140;

    GameTable_Top.EndX = 650;
    GameTable_Top.EndY = 140;

    GameTable_Top.Drawable[0].position.x = 150;
    GameTable_Top.Drawable[0].position.y = 140;

    GameTable_Top.Drawable[1].position.x = 650;
    GameTable_Top.Drawable[1].position.y = 140;

    GameTable_Top.Name = "Top";
    
    Table.push_back(&GameTable_Top);

    //Table Bottom
    Line GameTable_Bottom;
    GameTable_Bottom.StartX = 150;
    GameTable_Bottom.StartY = 410;

    GameTable_Bottom.EndX = 650;
    GameTable_Bottom.EndY = 410;

    GameTable_Bottom.Drawable[0].position.x = 150;
    GameTable_Bottom.Drawable[0].position.y = 410;

    GameTable_Bottom.Drawable[1].position.x = 650;
    GameTable_Bottom.Drawable[1].position.y = 410;

    GameTable_Bottom.Name = "Bottom";

    Table.push_back(&GameTable_Bottom);

    //Table Right
    Line GameTable_Right;
    GameTable_Right.StartX = 650;
    GameTable_Right.StartY = 140;

    GameTable_Right.EndX = 650;
    GameTable_Right.EndY = 410;

    GameTable_Right.Drawable[0].position.x = 650;
    GameTable_Right.Drawable[0].position.y = 140;

    GameTable_Right.Drawable[1].position.x = 650;
    GameTable_Right.Drawable[1].position.y = 410;

    GameTable_Right.Name = "Right";

    Table.push_back(&GameTable_Right);
    
    //Table Left
    Line GameTable_Left;
    GameTable_Left.StartX = 150;
    GameTable_Left.StartY = 140;

    GameTable_Left.EndX = 150;
    GameTable_Left.EndY = 410;

    GameTable_Left.Drawable[0].position.x = 150;
    GameTable_Left.Drawable[0].position.y = 140;

    GameTable_Left.Drawable[1].position.x = 150;
    GameTable_Left.Drawable[1].position.y = 410;

    GameTable_Left.Name = "Left";

    Table.push_back(&GameTable_Left);
   
    //Setup Click Bools + Clock
    
    bool LeftClick = false;
    bool RightClick = false;
    
    sf::Clock Clock;
    // Main loop that continues until we call window.close()
    while (window.isOpen())
    {
        // Handle any pending SFML events
        // These cover keyboard, mouse,joystick etc.
       
        sf::Event event;
        while (window.pollEvent(event))
        {
          switch(event.type)
          {
            case sf::Event::Closed:
              window.close();
            break;
            case sf::Event::MouseButtonPressed:
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    LeftClick = true;
                }
                if (event.mouseButton.button == sf::Mouse::Right)
                {
                    RightClick = true;
                }
                break;
            }
            case sf::Event::MouseButtonReleased:
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    LeftClick = false;
                }
                if (event.mouseButton.button == sf::Mouse::Right)
                {
                    RightClick = false;
                }
                break;
            }
            default:
              break;
          }

        }

        sf::Time Passed = Clock.restart();

      
      
        for (size_t i = 0; i < BallList.size(); i++)
         {
           //Balls_Update(BallList, Passed, ScreenWidth, ScreenHeight);

            //Edge Colisons
            for (size_t e = 0; e < Table.size(); e++)
            {
                Line* Edge = Table[e];
                Ball* Circle = BallList[i];

                float LineSegmentX = Edge->EndX - Edge->StartX;
                float LineSegmentY = Edge->EndY - Edge->StartY;

                float SegmentLength = (LineSegmentX * LineSegmentX) + (LineSegmentY * LineSegmentY);

                float LineToCircleX = Circle->Pos_X - Edge->StartX;
                float LineToCircleY = Circle->Pos_Y - Edge->StartY;

                float ColisonPoint = std::fmax(0, std::fmin(SegmentLength, (LineSegmentX * LineToCircleX + LineSegmentY * LineToCircleY))) / SegmentLength;

                float ColisonPointX = Edge->StartX + ColisonPoint * LineSegmentX;
                float ColisonPointY = Edge->StartY + ColisonPoint * LineSegmentY;

                float ColisonPointToCircle = sqrtf((Circle->Pos_X - ColisonPointX) * (Circle->Pos_X - ColisonPointX) + (Circle->Pos_Y - ColisonPointY) * (Circle->Pos_Y - ColisonPointY));

                if (Edge->Name == "Right" || Edge->Name == "Bottom")
                {
                    if (ColisonPointToCircle <= 1.75 * Circle->Radius)
                    {

                        Ball TempBall;
                        TempBall.Radius = Circle->Radius;
                        TempBall.Pos_X = ColisonPointX;
                        TempBall.Pos_Y = ColisonPointY;
                        TempBall.Mass = Circle->Mass;
                        TempBall.Name = "Temp";
                        TempBall.Color = sf::Color::Transparent;

                        TempBall.Velocity_X = 2 * Circle->Velocity_X;
                        TempBall.Velocity_Y = 2 * Circle->Velocity_Y;

                        EdgeColision(Circle, &TempBall);

                    }
                }
                else if (Edge->Name == "Left" || Edge->Name == "Top")
                {
                    if (ColisonPointToCircle <= 0.25 * Circle->Radius)
                    {

                        Ball TempBall;
                        TempBall.Radius = Circle->Radius;
                        TempBall.Pos_X = ColisonPointX;
                        TempBall.Pos_Y = ColisonPointY;
                        TempBall.Mass = Circle->Mass;
                        TempBall.Name = "Temp";
                        TempBall.Color = sf::Color::Transparent;

                        TempBall.Velocity_X = 2 * Circle->Velocity_X;
                        TempBall.Velocity_Y = 2 * Circle->Velocity_Y;

                        EdgeColision(Circle, &TempBall);
                        std::cout << Circle->Name << " Velocity X: " << Circle->Velocity_X << " Velocity Y: " << Circle->Velocity_Y << std::endl;
                    }
                }
            }

            for (size_t t = i + 1; t < BallList.size(); t++)
            {
                Ball* Ball_1 = BallList[i];
                Ball* Ball_2 = BallList[t];
                if (Ball_1->Name != Ball_2->Name)
                {
                    if (BallsOverlapTest(Ball_1->Pos_X, Ball_1->Pos_Y, Ball_1->Radius, Ball_2->Pos_X, Ball_2->Pos_Y, Ball_2->Radius))
                    {
                        //Colison Occurs

                        std::cout << "Static Colision Occured" << std::endl;
                        StaticColison(Ball_1, Ball_2);

                        std::cout << Ball_1->Name << " Velocity X: " << Ball_1->Velocity_X << " Velocity Y: " << Ball_1->Velocity_Y << std::endl;

                        std::cout << Ball_2->Name << " Velocity X: " << Ball_2->Velocity_X << " Velocity Y: " << Ball_2->Velocity_Y << std::endl;
                                
                        std::cout << "Moving Colision Occured" << std::endl;
                        MoivingColison(Ball_1, Ball_2);

                        std::cout << Ball_1->Name << " Velocity X: " << Ball_1->Velocity_X << " Velocity Y: " << Ball_1->Velocity_Y << std::endl;

                        std::cout << Ball_2->Name << " Velocity X: " << Ball_2->Velocity_X << " Velocity Y: " << Ball_2->Velocity_Y << std::endl;

                        std::cout << std::endl;
                    }
                }
            }
        }
            

        
        Balls_Update(BallList, Passed, ScreenWidth, ScreenHeight);
      



        sf::Vector2i MousePos = sf::Mouse::getPosition(window);
        if (RightClick)
        {
            for (Ball* i : BallList)
            {
                //Checks if the mouse pointer's positon exists within the radius of the circle
                if (fabs((MousePos.x - i->Pos_X) * (MousePos.x - i->Pos_X) + (MousePos.y - i->Pos_Y) * (MousePos.y - i->Pos_Y) < (i->Radius * i->Radius)))
                {
                    SelectedBall = i;
                    break;
                }
            }
       
        }
      

        if (SelectedBall!=nullptr && !RightClick)
        {
            SelectedBall->Velocity_X = 2.0f * ((SelectedBall->Pos_X) - (float(MousePos.x)));
            SelectedBall->Velocity_Y = 2.0f * ((SelectedBall->Pos_Y) - (float(MousePos.y)));
            
            SelectedBall = nullptr;
        }

        // We must clear the window each time around the loop
 
        
        sf::CircleShape Player(PlayerBall.Radius);
        Player.setPosition(PlayerBall.Pos_X, PlayerBall.Pos_Y);
        Player.setFillColor(PlayerBall.Color);

        sf::CircleShape Other_1(OtherBall_1.Radius);
        Other_1.setPosition(OtherBall_1.Pos_X, OtherBall_1.Pos_Y);
        Other_1.setFillColor(OtherBall_1.Color);

        sf::CircleShape Other_2(OtherBall_2.Radius);
        Other_2.setPosition(OtherBall_2.Pos_X, OtherBall_2.Pos_Y);
        Other_2.setFillColor(OtherBall_2.Color);

        window.clear();
        window.draw(Player);
        window.draw(Other_1);
        window.draw(Other_2);
        window.draw(GameTable_Top.Drawable,2,sf::Lines);
        window.draw(GameTable_Bottom.Drawable, 2, sf::Lines);
        window.draw(GameTable_Right.Drawable, 2, sf::Lines);
        window.draw(GameTable_Left.Drawable, 2, sf::Lines);

        if (SelectedBall != nullptr && sf::Mouse::isButtonPressed(sf::Mouse::Right))
        {
            sf::Vertex Line[] =
            {
                sf::Vertex(sf::Vector2f(SelectedBall->Pos_X, SelectedBall->Pos_Y)),
                sf::Vertex(sf::Vector2f(MousePos.x, MousePos.y))
            };
            window.draw(Line, 2, sf::Lines);
        }
        // Get the window to display its contents
        window.display();
    }

    

    return 0;
}
