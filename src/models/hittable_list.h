#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include "aabb.h"
#include "hittable.h"

#include <vector>

class HittableList : public Hittable
{
public:
    std::vector<shared_ptr<Hittable>> objects;

    HittableList() {}
    HittableList(shared_ptr<Hittable> object) { add(object); }

    void clear() { objects.clear(); }

    void add(shared_ptr<Hittable> object)
    {
        object->id = _cur_max_id;
        objects.push_back(object);
        bbox = Aaab(bbox, object->bounding_box());
        _cur_max_id++;
    }

    shared_ptr<Hittable> get_object_by_id(size_t id)
    {
        for (auto object : objects)
        {
            if (object->id == id)
            {
                return object;
            }
        }
        return nullptr;
    }

    // Функция вставялет элемент за место
    void add_element_instead_of_id(size_t id, std::shared_ptr<Hittable> new_object)
    {
        new_object->id = id;
        for (std::shared_ptr<Hittable> &object : objects)
        {
            if (object->id == id)
            {
                std::cout << "Тут" << std::endl;
                object = new_object;
                return;
            }
        }
        throw std::invalid_argument("В списке нет объекта с таким id");
    }

    void delete_object(size_t id)
    {
        auto it = std::find_if(objects.begin(), objects.end(),
                               [id](const auto &object)
                               {
                                   return object->id == id;
                               });

        if (it != objects.end())
        {
            objects.erase(it);
        }
    }

    bool hit(const Ray &r, Interval ray_t, HitRecord &rec) const override
    {
        HitRecord temp_rec;
        bool hit_anything = false;
        auto closest_so_far = ray_t.max;

        for (const auto &object : objects)
        {
            if (object->hit(r, Interval(ray_t.min, closest_so_far), temp_rec))
            {
                hit_anything = true;
                closest_so_far = temp_rec.t;
                rec = temp_rec;
            }
        }

        return hit_anything;
    }

    Aaab bounding_box() const override { return bbox; }

    std::vector<size_t> get_objects_ids()
    {
        std::vector<size_t> ids;

        for (auto object : objects)
        {
            ids.push_back(object->id);
        }

        return ids;
    }

private:
    Aaab bbox;
    size_t _cur_max_id = 0;
};

#endif
