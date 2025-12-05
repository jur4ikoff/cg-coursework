#pragma once
#include <memory>

class Tile
{

public:
    Tile(int _i, int _j, int _end_i, int _end_j)
    {
        i = _i;
        j = _j;
        end_i = _end_i;
        end_j = _end_j;
    }
    int i;
    int j;
    int end_i;
    int end_j;
};

class TileCreator
{
public:
    TileCreator(int size_x, int size_y, int step = 100)
        : _size_x(size_x), _size_y(size_y) {}

    std::vector<std::shared_ptr<Tile>> create(int step)
    {
        if (step < 20)
        {
            step = 20;
        }
        std::vector<std::shared_ptr<Tile>> tiles;
        for (int i = 0; i < _size_x; i += step)
        {
            for (int j = 0; j < _size_y; j += step)
            {

                auto tile = std::make_shared<Tile>(i, j, std::min(i + step, _size_x),
                                                   std::min(j + step, _size_y));
                tiles.push_back(tile);
            }
        }
        return tiles;
    }

private:
    int _size_x;
    int _size_y;
};
