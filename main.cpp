#include<iostream>
#include<raylib.h>
#include<deque>
#include<raymath.h>

using namespace std;

enum GameState{MENU, GAMEPLAY, GAMEOVER};
enum Level{EZ, MEDIUM, HARD};
GameState currentState =MENU;
Level currentLevel = MEDIUM;
Rectangle ezButton;
Rectangle mediumButton;
Rectangle hardButton;
Rectangle restartButton;

bool running=true;

//create the time the snake move, it necessary beacause the snake move 60 time per second so we need to made it slowdown
double last_time=0;
double choose_level_speed=0.1;
bool event_begin(double cmp){
    double current_time= GetTime();
    if(current_time-last_time>cmp){
        last_time=current_time;
        return true;
    }
    return false;
}

bool Food_on_top_of_snake(Vector2 element, deque<Vector2> deque){
    for(unsigned int i=0 ;i<deque.size(); i++){
        if(Vector2Equals(deque[i],element)){
            return true;
        }
    }
    return false;
} // this function is use to check if the new food position appear on top of the snake, it will reture true if yes and false if no 


Color green ={110,200,140,255};
Color red ={230,100,100,255};
int CellSize =30; // we use this to represent the grid that *cellcount mean size 30 and have 25 cell
int CellCount=25;
int score=0;

class Sound_Game{
    public:
    Music music;
    Sound SoundEat;
    Sound SoundLose;

    Sound_Game(){
    InitAudioDevice();
    music=LoadMusicStream("sound/theme.mp3");
    SoundEat=LoadSound("sound/eat.mp3");
    SoundLose=LoadSound("sound/lose.mp3");
    PlayMusicStream(music);
    }

    ~Sound_Game(){
        UnloadMusicStream(music);
        UnloadSound(SoundEat);
        CloseAudioDevice();
    }
};

Sound_Game sound;


// Creat the food
class Food{
    public:
    // create vector2 to save the position of food
    Vector2 position;
    Texture2D texture;


    Food(deque<Vector2> snakeBody){
        Image image= LoadImage("Image/new2Donut.png"); // with picture you down on web, you need to open it on paint and then resize it and save as .png
        ImageColorReplace(&image, WHITE, BLANK);   // this line of code use to delete the white background of image 
        texture=LoadTextureFromImage(image);
        UnloadImage(image);
        position = GetRandomPostion(snakeBody);
    }

    ~Food(){
        UnloadTexture(texture);
    }

    void Draw(){
        DrawTexture(texture,position.x*CellSize, position.y*CellSize,WHITE);
    }

    Vector2 GeneratedRandomCell(){
        float x = GetRandomValue(0,CellCount-1);
        float y = GetRandomValue(0, CellCount-1);
       return Vector2{x, y};
    }

    Vector2 GetRandomPostion(deque<Vector2> snakeBody){
           Vector2 position = GeneratedRandomCell();   
           while(Food_on_top_of_snake(position, snakeBody)){
                position =GeneratedRandomCell();
           }       
           return position;
    }


};

class Snake {
    public:
    bool snakeGrow=false;
    Texture2D texture;
    Snake(){
  
    Image image =LoadImage("Image/snake.png");
    ImageColorReplace(&image,WHITE, BLANK);
    texture=LoadTextureFromImage(image);
    UnloadImage(image);
    }
     ~Snake(){
            UnloadTexture(texture);
    }

    deque<Vector2> body ={Vector2{6,9},Vector2{5,9},Vector2{4,9}};
    Vector2 direction ={1,0};
    void Draw(){
        for( unsigned  i=0; i<body.size();i++){
            float x=body[i].x;
            float y=body[i].y;
            //Rectangle segment ={x*CellSize, y*CellSize, (float)CellSize,(float)CellSize};
            //DrawRectangleRounded(segment, 0.5, 10, red);
            DrawTexture(texture,x*CellSize,y*CellSize,WHITE);

        }
        
    }
   
    void Move(){
        if(IsKeyPressed(KEY_UP) && direction.y !=1){
            direction={0,-1};
        }else if(IsKeyPressed(KEY_DOWN) && direction.y!=-1){
            direction={0,1};
        }else if(IsKeyPressed(KEY_RIGHT) && direction.x!=-1){
            direction={1,0};
           
        }else if(IsKeyPressed(KEY_LEFT) && direction.x!=1){
            direction={-1,0};
           }
    }


    void Update(){
        UpdateMusicStream(sound.music);
        if(snakeGrow==true){
            body.push_front(Vector2Add(body[0],direction));
            snakeGrow=false; // if dont have this, the snake is growing uncontrol
        }else{
            body.pop_back();
            body.push_front(Vector2Add(body[0],direction));
        }
    }

};


class Game{
      
    public:
        Food food = Food(snake.body);
        Snake snake= Snake();

        void Draw(){
            food.Draw();
            snake.Draw();
        }

        void Update(){
           
            snake.Update();
            checkCollisonWithFood();
            checkCollisionWithEdges();
            checkCollisionWithBody();
            
            
        }

        void Move(){
            if(running ==true){
                snake.Move();
            }
            
        }

        void checkCollisonWithFood(){
            if(Vector2Equals(snake.body[0],food.position)){
                food.position =food.GetRandomPostion(snake.body);
                snake.snakeGrow=true;
                score++;
                PlaySound(sound.SoundEat);
            }
        }

        
        void checkCollisionWithEdges(){
            if(snake.body[0].x==CellCount || snake.body[0].x==-1){
                PlaySound(sound.SoundLose);
                gameOver();
            }
            if(snake.body[0].y==CellCount || snake.body[0].y==-1){
                PlaySound(sound.SoundLose);
                gameOver();
            }
        }

        void gameOver(){
                reStart();
        }

        void reStart(){
            snake.body={Vector2{6,9},Vector2{5,9},Vector2{4,9}};
             score=0;
             running =false;
            currentState=GAMEOVER;
           
        }

        
        void checkCollisionWithBody(){
            deque<Vector2> FakeBody=snake.body;
            FakeBody.pop_front();
            if(Food_on_top_of_snake(snake.body[0],FakeBody)){
                PlaySound(sound.SoundLose);
                reStart();
            }  // this reuse the function before 
        }


};



int main(){

    InitWindow(CellSize*CellCount, CellSize*CellCount,"Snake Game");
    SetTargetFPS(60); 
    ezButton = {(CellSize*CellCount/2.0f)-150,250,250,60};
    mediumButton = {(CellSize*CellCount/2.0f)-150,350,250,60};
    hardButton = {(CellSize*CellCount/2.0f)-150,450,250,60};
    restartButton={(CellSize*CellCount/2.0f)-200,300,400,60};
    Game game = Game();
    while(!WindowShouldClose()){
        if(currentState==MENU){
            if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
                Vector2 mousepoint =GetMousePosition();
                if(CheckCollisionPointRec(mousepoint,ezButton)){
                    currentLevel=EZ;
                    choose_level_speed=0.15;
                    event_begin(choose_level_speed);
                    currentState=GAMEPLAY;
                }else if(CheckCollisionPointRec(mousepoint,mediumButton)){
                    currentLevel=MEDIUM;
                    choose_level_speed=0.085;
                    event_begin(choose_level_speed);
                    currentState=GAMEPLAY;
                }else if(CheckCollisionPointRec(mousepoint,hardButton)){
                    currentLevel=HARD;
                    choose_level_speed=0.045;
                    event_begin(choose_level_speed);
                    currentState=GAMEPLAY;
                }
            }
       
   
        } else if(currentState==GAMEPLAY){
            running =true;
            game.Move();
           if(event_begin(choose_level_speed)){
            game.Update();
           }
        } else if(currentState==GAMEOVER){
            if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
                Vector2 mousepoint =GetMousePosition();
                if(CheckCollisionPointRec(mousepoint,restartButton)){
                    currentState=MENU;
                }
            }
        }
        BeginDrawing(); 
        ClearBackground(green);
        DrawText(TextFormat("%i",score),CellSize*CellCount/3-20,20,80,WHITE);
        if(currentState==MENU){
            DrawText("CHON CHE DO CHOI",CellCount*CellSize/2.0f -MeasureText("CHON CHE DO CHOI",40)/2,120,40,WHITE);
            DrawRectangleRec(ezButton,LIGHTGRAY);
            DrawText("EZ",(int)ezButton.x+85,(int)ezButton.y+15,30,DARKGRAY);

            DrawRectangleRec(mediumButton,LIGHTGRAY);
            DrawText("MEDIUM",(int)mediumButton.x+75,(int)mediumButton.y+15,30,DARKGRAY);

            DrawRectangleRec(hardButton,LIGHTGRAY);
            DrawText("HARD",(int)hardButton.x+85,(int)hardButton.y+15,30,DARKGRAY);
        }else if(currentState==GAMEPLAY){
            game.Draw();
           
        }else if(currentState==GAMEOVER){
            DrawRectangleRec(restartButton,LIGHTGRAY);
            DrawText("RESTART",(int)restartButton.x+80,(int)restartButton.y+15,50,DARKBLUE);
        }
       
        
        
       
        EndDrawing();
    }
    CloseWindow();
    return 0;
}