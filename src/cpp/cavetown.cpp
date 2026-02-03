
#include <vector>
#include <random>
#include <iostream>
#include <fstream>
#include <string>

class CaveGenerator {
private:
    int width;
    int height;
    std::vector<std::vector<int>> grid;
    std::mt19937 rng;

public:
    CaveGenerator(int w, int h, unsigned int seed = 42) 
        : width(w), height(h), rng(seed) {
        grid.resize(height, std::vector<int>(width, 0));
    }

    void initialize(float wall_probability = 0.45f) {
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                if (x == 0 || x == width-1 || y == 0 || y == height-1) {
                    grid[y][x] = 1;
                } else {
                    grid[y][x] = (dist(rng) < wall_probability) ? 1 : 0;
                }
            }
        }
    }

    int countNeighborWalls(int x, int y, int range = 1) {
        int count = 0;
        for (int dy = -range; dy <= range; dy++) {
            for (int dx = -range; dx <= range; dx++) {
                if (dx == 0 && dy == 0) continue;
                
                int nx = x + dx;
                int ny = y + dy;
           
                if (nx < 0 || nx >= width || ny < 0 || ny >= height) {
                    count++;
                } else {
                    count += grid[ny][nx];
                }
            }
        }
        return count;
    }

 
    void smooth(int iterations = 5) {
        for (int iter = 0; iter < iterations; iter++) {
            std::vector<std::vector<int>> newGrid = grid;
            
            for (int y = 1; y < height-1; y++) {
                for (int x = 1; x < width-1; x++) {
                    int neighbors = countNeighborWalls(x, y, 1);
                    
                   
                    if (neighbors >= 5) {
                        newGrid[y][x] = 1;
                    } else if (neighbors <= 3) {
                        newGrid[y][x] = 0;
                    }
                }
            }
            
            grid = newGrid;
        }
    }

    void addObstacles(int num_obstacles = 10) {
        std::uniform_int_distribution<int> distX(5, width-5);
        std::uniform_int_distribution<int> distY(5, height-5);
        std::uniform_int_distribution<int> distSize(1, 3);
        
        for (int i = 0; i < num_obstacles; i++) {
            int cx = distX(rng);
            int cy = distY(rng);
            int size = distSize(rng);
            
          
            if (grid[cy][cx] == 0) {
                for (int dy = -size; dy <= size; dy++) {
                    for (int dx = -size; dx <= size; dx++) {
                        int x = cx + dx;
                        int y = cy + dy;
                        if (x >= 0 && x < width && y >= 0 && y < height) {
                            if (dx*dx + dy*dy <= size*size) {
                                grid[y][x] = 1;
                            }
                        }
                    }
                }
            }
        }
    }

    void generate() {
        initialize(0.45f);
        smooth(5);
        addObstacles(8);
    }

    int get(int x, int y) const {
        if (x < 0 || x >= width || y < 0 || y >= height) return 1;
        return grid[y][x];
    }

  
    void print() const {
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                std::cout << (grid[y][x] ? '#' : '.');
            }
            std::cout << '\n';
        }
    }

  
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    
   
    const std::vector<std::vector<int>>& getGrid() const { return grid; }

   
    void saveToJson(const std::string& filename) const {
        std::ofstream outFile(filename);
        if (outFile.is_open()) {
            outFile << "{\n";
            outFile << "  \"width\": " << width << ",\n";
            outFile << "  \"height\": " << height << ",\n";
            outFile << "  \"grid\": [\n";
            for (int y = 0; y < height; y++) {
                outFile << "    [";
                for (int x = 0; x < width; x++) {
                    outFile << grid[y][x] << (x == width - 1 ? "" : ",");
                }
                outFile << "]" << (y == height - 1 ? "" : ",") << "\n";
            }
            outFile << "  ]\n";
            outFile << "}\n";
            outFile.close();
            std::cout << "Cave grid exported to " << filename << "\n";
        } else {
            std::cerr << "Unable to open file: " << filename << "\n";
        }
    }
};

int main() {
    std::cout << "Cave Generator Test\n";
    std::cout << "===================\n\n";
    
    CaveGenerator cave(50, 25);
    cave.generate();
    cave.print();
    cave.saveToJson("cave_data.json");
    
    return 0;
}