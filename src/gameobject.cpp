#include "gameobject.h"

void GameObject::BFS_Collision(Player & player, Vector2 & playerCenter, Vector2 nextTile, std::vector<std::vector<bool>> & trace, std::queue<Vector2> & Q){
    float maxDist = Screen::player_size*sqrt(61)/6-2;
    float eps1 = 1e-2;
    float eps2 = eps1 * 7;
    Vector2 nextCenter = (nextTile + Vector2(0.5)) * Screen::player_size;
    float h = playerCenter.distance(nextCenter);
    if (Vector2::IsInRange(nextTile, 0, 15, 0, 15) &&
        !trace[nextTile.x][nextTile.y] &&
        h <= maxDist
    )
    {
        int type = Game::map[nextTile.y][nextTile.x];
        if (type){
            SDL_RenderDrawLine(Game::renderer, playerCenter.x, playerCenter.y, nextCenter.x, nextCenter.y);
        }
        if (type==1){
            Renderer::SetDrawColor(Color::white(255));
            int direction = 0;
            Vector2 d = nextCenter - playerCenter;
            float cos = d.x/h;
            float angle = acos(cos);
            float range = atan(6.0/5.0);
            if ((angle < range - eps1) || (abs(cos-1) <= eps1)){
                direction = 1;
            }
            else if ((M_PI - range + eps1 < angle && angle < M_PI - eps1) || (abs(cos+1) <= eps1)){
                direction = 2;
            }
            else if ((nextCenter.y > playerCenter.y) && ((range + eps2 < angle && angle < M_PI - range - eps2) || (abs(cos) <= eps2))){
                direction = 3;
            }
            else if ((nextCenter.y < playerCenter.y) && ((range + eps2 < angle && angle < M_PI - range - eps2) || (abs(cos) <= eps2))){
                direction = 4;
            }
            if (Rect::isCollide(playerCenter, Vector2(player.size.x/6*4, player.size.y), nextCenter, player.size)){
                switch (direction){
                    case 1:
                        player.position.x = (nextTile.x - 5.0/6.0) * player.size.x;
                        break;
                    case 2:
                        player.position.x = (nextTile.x + 5.0/6.0) * player.size.x;
                        break;
                    case 3:
                        player.position.y = (nextTile.y - 1) * player.size.y;
                        player.tmp_collide_down = true;
                        break;
                    case 4:
                        player.position.y = (nextTile.y + 1) * player.size.y;
                        player.tmp_collide_up = true;
                        break;
                    default:
                        break;
                }
            }
        }
        else if (type==2){
            Renderer::SetDrawColor(Color::yellow(255));
            if (Rect::isCollide(playerCenter, Vector2(player.size.x/6*4, player.size.y), nextCenter, player.size)){
                Game::map[nextTile.y][nextTile.x] = 0;
            }
        }
        trace[nextTile.x][nextTile.y] = 1;
        Q.push(nextTile);
    }
}