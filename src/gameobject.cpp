#include "gameobject.h"
#include "game.h"

void GameObject::BFS_Collision(Player & player, Vector2 & playerCenter, Vector2 nextTile, std::unordered_map<Vector2, bool, Vector2Hash, Vector2Equal> & visit, std::queue<Vector2> & Q, float maxDist, float eps){
    Vector2 nextCenter = (nextTile + Vector2(0.5)) * Screen::player_size;
    float h = playerCenter.distance(nextCenter);
    if (IsV2InRange(nextTile, Vector2(), Vector2(15)) &&
        visit.find(nextTile) == visit.end() &&
        h <= maxDist)
    {
        int type = tilemap[nextTile.y][nextTile.x];
        if (type & (WALL | DAMAGE)){
            Screen::SetDrawColor(Color::white(255));
            Vector2 d = nextCenter - playerCenter;
            float cos = d.x/h;
            float angle = acos(cos);
            float range = atan(6.0/5.0);
            if ((angle < range - eps) || (abs(cos-1) <= eps)){
                player.position.x = std::min(player.position.x, (float)(nextTile.x - 5.0/6.0) * player.size.x - 1);
            }
            else if ((M_PI - range + eps < angle && angle < M_PI - eps) || (abs(cos+1) <= eps)){
                player.position.x = std::max(player.position.x, (float)(nextTile.x + 5.0/6.0) * player.size.x + 1);
            }
            else if ((nextCenter.y > playerCenter.y) && ((range + eps * 5 < angle && angle < M_PI - range - eps * 5) || (abs(cos) <= eps))){
                player.position.y = std::min(player.position.y, (float)(nextTile.y - 1) * player.size.y);
                if (abs(playerCenter.y - nextCenter.y) - player.size.y <= 3) player.tmp_collide_down = true;
            }
            else if ((nextCenter.y < playerCenter.y) && ((range + eps * 5 < angle && angle < M_PI - range - eps * 5) || (abs(cos) <= eps))){
                player.position.y = std::max(player.position.y, (float)(nextTile.y + 1) * player.size.y);
                if (abs(playerCenter.y - nextCenter.y) - player.size.y <= 0) player.tmp_collide_up = true;
            }
            
            if (type & DAMAGE){
                Screen::SetDrawColor(Color::red(255));
                // ShowMsg(0, logging, "player was damaged.");
            }
        }
        else if (type & COIN){
            Screen::SetDrawColor(Color::yellow(255));
            if (Rect::isCollide(playerCenter, Vector2(player.size.x/6*4, player.size.y), nextCenter, player.size)){
                Game::player_score++;
                ShowMsg(0, logging, "player score: " + std::to_string(Game::player_score) + ".");
                tilemap[nextTile.y][nextTile.x] = 0;
                playSound("coin", coin_channel, 0);
            }
        }
        else if (type & WIN){
            Screen::SetDrawColor(Color::green(255));
            if (Rect::isCollide(playerCenter, Vector2(player.size.x/6*4, player.size.y), nextCenter, player.size)){
                ShowMsg(0, logging, "player won!");
            }
        }
        else if (type & SPAWN){
            Screen::SetDrawColor(Color::cyan(255));
        }
        if (type){
            SDL_RenderDrawLine(Game::renderer, playerCenter.x, playerCenter.y, nextCenter.x, nextCenter.y);
        }
        visit.insert({nextTile, 1});
        Q.push(nextTile);
    }
}