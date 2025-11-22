"""
Procedural Texture Generator for City Designer
Run this script to generate texture files for the project.
Requirements: pip install Pillow numpy
"""

from PIL import Image, ImageDraw
import random
import os

def create_building_texture(filename, width=512, height=512, style=1):
    """Create a building facade texture with windows"""
    img = Image.new('RGB', (width, height), color=(80, 80, 90))
    draw = ImageDraw.Draw(img)
    
    # Window grid
    window_width = 30
    window_height = 40
    spacing_x = 45
    spacing_y = 55
    
    for y in range(20, height - 20, spacing_y):
        for x in range(20, width - 20, spacing_x):
            # Window color (lit or dark)
            if random.random() > 0.3:
                color = (255, 255, 200) if style == 1 else (220, 240, 255)
            else:
                color = (40, 40, 50)
            
            draw.rectangle([x, y, x + window_width, y + window_height], 
                          fill=color, outline=(60, 60, 70))
    
    img.save(filename)
    print(f"Created {filename}")

def create_road_texture(filename, width=512, height=512):
    """Create a road texture with asphalt and lane markings"""
    img = Image.new('RGB', (width, height), color=(50, 50, 55))
    draw = ImageDraw.Draw(img)
    
    # Add some noise for asphalt texture
    pixels = img.load()
    for y in range(height):
        for x in range(width):
            noise = random.randint(-15, 15)
            val = 50 + noise
            pixels[x, y] = (val, val, val + 5)
    
    # Lane markings
    dash_height = 40
    gap = 30
    line_width = 8
    
    for y in range(0, height, dash_height + gap):
        draw.rectangle([width // 2 - line_width // 2, y, 
                       width // 2 + line_width // 2, y + dash_height],
                      fill=(255, 255, 255))
    
    img.save(filename)
    print(f"Created {filename}")

def create_grass_texture(filename, width=512, height=512):
    """Create a grass texture"""
    img = Image.new('RGB', (width, height), color=(40, 100, 40))
    pixels = img.load()
    
    # Add grass variation
    for y in range(height):
        for x in range(width):
            r = 30 + random.randint(0, 30)
            g = 80 + random.randint(0, 40)
            b = 30 + random.randint(0, 20)
            pixels[x, y] = (r, g, b)
    
    img.save(filename)
    print(f"Created {filename}")

def create_fountain_texture(filename, width=256, height=256):
    """Create a fountain texture with water-like appearance"""
    img = Image.new('RGBA', (width, height), color=(200, 220, 255, 255))
    draw = ImageDraw.Draw(img)
    pixels = img.load()
    
    # Add water shimmer effect
    for y in range(height):
        for x in range(width):
            r = 180 + random.randint(0, 40)
            g = 200 + random.randint(0, 40)
            b = 230 + random.randint(0, 25)
            a = 255
            pixels[x, y] = (r, g, b, a)
    
    # Add some circular patterns
    center_x, center_y = width // 2, height // 2
    for radius in range(20, 100, 20):
        color = (255, 255, 255, 100)
        draw.ellipse([center_x - radius, center_y - radius,
                     center_x + radius, center_y + radius],
                    outline=color, width=2)
    
    img.save(filename)
    print(f"Created {filename}")

if __name__ == "__main__":
    # Create assets directory if it doesn't exist
    os.makedirs("assets", exist_ok=True)
    
    print("Generating textures...")
    create_building_texture("assets/building1.jpg", style=1)
    create_building_texture("assets/building2.jpg", style=2)
    create_road_texture("assets/road.jpg")
    create_grass_texture("assets/grass.jpg")
    create_fountain_texture("assets/fountain.png")
    
    print("\nAll textures generated successfully!")
    print("You can now build and run the project.")
