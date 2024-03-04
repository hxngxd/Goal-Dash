#include "gameobject.h"

void GameObject::BFS_Collision(Player & player, Vector2 & playerCenter, Vector2 nextTile, std::unordered_map<Vector2, bool, Vector2Hash, Vector2Equal> & visit, std::queue<Vector2> & Q, float maxDist, float eps){
    Vector2 nextCenter = (nextTile + Vector2(0.5)) * Screen::player_size;
    float h = playerCenter.distance(nextCenter);
    if (Vector2::IsInRange(nextTile, 0, 15, 0, 15) &&
        visit.find(nextTile) == visit.end() &&
        h <= maxDist
    )
    {
        int type = Game::map[nextTile.y][nextTile.x];
        if (type==1 || type==3){
            Renderer::SetDrawColor(Color::white(255));
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
        }
        else if (type==2){
            Renderer::SetDrawColor(Color::yellow(255));
            if (Rect::isCollide(playerCenter, Vector2(player.size.x/6*4, player.size.y), nextCenter, player.size)){
                Game::map[nextTile.y][nextTile.x] = 0;
            }
        }
        if (Game::view_mode && type){
            SDL_RenderDrawLine(Game::renderer, playerCenter.x, playerCenter.y, nextCenter.x, nextCenter.y);
        }
        visit.insert({nextTile, 1});
        Q.push(nextTile);
    }
}