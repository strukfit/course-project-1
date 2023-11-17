#include "SQLController.h"

using namespace std::string_literals;

SQLController::SQLController() :
    db(nullptr)
{

}

SQLController::~SQLController()
{
    if (db) 
    {
        sqlite3_close(db);
    }
}

bool SQLController::isDatabaseExist(const std::string& dbName)
{
    std::ifstream file(dbName);
    return file.good();
}

void SQLController::OpenDB(const char* filename)
{
    if (sqlite3_open(filename, &db))
    {
        throw(Exception("Can't open database: "s + sqlite3_errmsg(db)));
    }
}

void SQLController::ExecuteSQL(const char* sql)
{
    sqlite3_stmt* stmt;

    char* errMsg = 0;
    if (sqlite3_exec(db, sql, 0, 0, nullptr) != SQLITE_OK) 
    {
        throw(Exception("Error when executing SQL query: "s + sqlite3_errmsg(db)));
    }
}

sqlite3_stmt* SQLController::PrepareSQL(const char* sql)
{
    sqlite3_stmt* stmt;
    // Preparing SQL statement(compilation into a byte-code program) 
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK)
    {
        throw(Exception("Error in preparation of SQL query: "s + sqlite3_errmsg(db)));
    }

    return stmt;
}

void SQLController::DatabaseInit()
{
    ExecuteSQL(R"(
        CREATE TABLE IF NOT EXISTS Products (
            ProductID INTEGER PRIMARY KEY AUTOINCREMENT,
            ProductName TEXT NOT NULL UNIQUE,
            Description TEXT,
            Price REAL,
            StockQuantity INTEGER,
            CategoryID INTEGER,
            ManufacturerID INTEGER,
            FOREIGN KEY (CategoryID) REFERENCES ProductCategories(CategoryID),
            FOREIGN KEY (ManufacturerID) REFERENCES Manufacturers(ManufacturerID)
        );

        CREATE TABLE IF NOT EXISTS ProductCategories (
            CategoryID INTEGER PRIMARY KEY AUTOINCREMENT,
            CategoryName TEXT NOT NULL UNIQUE,
            AverageRating REAL,
            Description TEXT
        );

        CREATE TABLE IF NOT EXISTS Manufacturers (
            ManufacturerID INTEGER PRIMARY KEY AUTOINCREMENT,
            Name TEXT NOT NULL UNIQUE,
            CountryOfOrigin TEXT,
            ContactInfo TEXT,
            WebsiteURL TEXT,
            ProductionFacilities TEXT,
            Certifications TEXT
        );        
        )"
    );

    Test();
}

sqlite3_stmt* SQLController::SelectData(const char* tableName)
{
    std::string selectData = "SELECT * FROM "s + tableName;
    
    return PrepareSQL(selectData.c_str());
}

void SQLController::Test()
{
    ExecuteSQL(R"(
        INSERT INTO ProductCategories (CategoryName, AverageRating, Description) 
        VALUES
            ('Kitchen Appliances', 4.2, 'Appliances for cooking at home'),
            ('Cleaning Appliances', 3.8, 'Everything you need for cleaning'),
            ('Home Electronics', 4.5, 'Devices for entertainment and communication'),
            ('Household Gadgets', 4.0, 'Various household gadgets for convenience'),
            ('Sleeping Aids', 4.1, 'Devices for a comfortable sleep'),
            ('Audio and Video Equipment', 4.4, 'Devices for audio and video entertainment'),
            ('Climate Control Equipment', 4.3, 'Devices for creating a comfortable microclimate'),
            ('Kitchen Gadgets', 3.9, 'Interesting gadgets for the kitchen'),
            ('Lighting Equipment', 4.2, 'Equipment for lighting rooms'),
            ('Group Exercise Equipment', 4.6, 'Equipment for sports and fitness'),
            ('Bathroom Equipment', 3.7, 'Devices for comfort in the bathroom'),
            ('Pet Care Equipment', 3.8, 'Devices for taking care of pets'),
            ('Document Processing Equipment', 4.3, 'Office equipment for document processing');

        INSERT INTO Manufacturers (Name, CountryOfOrigin, ContactInfo, WebsiteURL, ProductionFacilities, Certifications) 
        VALUES
            ('ABC Electronics', 'USA', 'info@abc-electronics.com', 'www.abc-electronics.com', '123 Main St, Anytown, USA', 'ISO 9001'),
            ('Tech Innovators Ltd.', 'UK', 'info@tech-innovators.co.uk', 'www.tech-innovators.co.uk', '456 High St, Tech City, UK', 'CE, RoHS'),
            ('Home Appliance Solutions', 'Canada', 'info@hasolutions.ca', 'www.hasolutions.ca', '789 Maple Ave, Appliance Town, Canada', 'UL Certified'),
            ('Global Gadgets Corp.', 'China', 'info@global-gadgets.cn', 'www.global-gadgets.cn', '321 Elm St, Gadget City, China', 'ISO 14001'),
            ('Comfort Living Appliances', 'Germany', 'info@comfort-living.de', 'www.comfort-living.de', '567 Oak St, Living Town, Germany', 'CE, TUV'),
            ('EcoTech Solutions', 'Australia', 'info@ecotech-solutions.com.au', 'www.ecotech-solutions.com.au', '890 Pine St, Eco City, Australia', 'Green Label'),
            ('InnoTech Electronics', 'Japan', 'info@inno-tech.jp', 'www.inno-tech.jp', '234 Cedar St, Innovation City, Japan', 'ISO 9001, JIS Certified'),
            ('Smart Home Creations', 'South Korea', 'info@smarthome-creations.kr', 'www.smarthome-creations.kr', '567 Birch St, Smart City, South Korea', 'KC Mark'),
            ('Appliance Masterworks', 'Italy', 'info@appliance-masterworks.it', 'www.appliance-masterworks.it', '789 Walnut St, Masterworks Town, Italy', 'CE, RoHS'),
            ('Tech Bliss Innovations', 'India', 'info@tech-bliss.in', 'www.tech-bliss.in', '123 Mahogany St, Bliss City, India', 'ISO 9001'),
            ('InnoGadget Co.', 'Singapore', 'info@innogadget.sg', 'www.innogadget.sg', '456 Teak St, Gadget Square, Singapore', 'ISO 14001'),
            ('Home Harmony Appliances', 'Brazil', 'info@home-harmony.com.br', 'www.home-harmony.com.br', '789 Rosewood St, Harmony Town, Brazil', 'UL Certified'),
            ('AquaTech Solutions', 'Netherlands', 'info@aquatech-solutions.nl', 'www.aquatech-solutions.nl', '234 Linden St, Aqua City, Netherlands', 'CE, TUV'),
            ('Tech Trends Ltd.', 'France', 'info@tech-trends.fr', 'www.tech-trends.fr', '567 Spruce St, Tech Trends City, France', 'ISO 9001, CE'),
            ('Green Living Electronics', 'Switzerland', 'info@green-living.ch', 'www.green-living.ch', '890 Fir St, Green City, Switzerland', 'Green Label');
        
        -- Products for Kitchen Appliances
        INSERT INTO Products (ProductName, Description, Price, StockQuantity, CategoryID, ManufacturerID) 
        VALUES
            ('Smart Blender 2000', 'High-tech blender for smoothies and shakes', 129.99, 50, 1, 7),
            ('Digital Toaster Pro', 'Smart toaster with multiple toasting options', 59.99, 30, 1, 8),
            ('Coffee Master Plus', 'Automatic coffee maker with built-in grinder', 199.99, 40, 1, 8),
            ('Multi-Cooker Deluxe', 'Versatile multi-cooker for easy meal preparation', 179.99, 25, 1, 5),
            ('Digital Air Fryer', 'Healthy cooking with less oil using digital air frying technology', 149.99, 35, 1, 3),

        -- Products for Cleaning Appliances
            ('RoboClean 5000', 'Robotic vacuum cleaner with AI navigation', 299.99, 20, 2, 6),
            ('Power Scrubber Pro', 'Electric scrubber for efficient cleaning', 79.99, 40, 2, 8),
            ('Smart Mop 2.0', 'Intelligent mop with self-cleaning feature', 49.99, 50, 2, 13),
            ('Ultra-Duster Max', 'High-power duster for quick and effective dusting', 39.99, 30, 2, 12),
            ('Window Wizard', 'Automatic window cleaning robot for streak-free windows', 129.99, 25, 2, 14),

        -- Products for Home Electronics
            ('Home Theater System', 'Immersive sound experience for your living room', 499.99, 10, 3, 1),
            ('Smart Home Hub', 'Centralized hub for controlling smart home devices', 149.99, 20, 3, 5),
            ('Wireless Speaker Set', 'High-quality wireless speakers for audio enthusiasts', 249.99, 25, 3, 11),
            ('Digital Photo Frame', 'Digital frame for displaying your favorite photos', 79.99, 30, 3, 14),

        -- Products for Household Gadgets
            ('Smart Home Assistant', 'Voice-activated home assistant for smart living', 129.99, 20, 4, 8),
            ('Intelligent Iron', 'Smart iron with temperature control and safety features', 69.99, 30, 4, 9),
            ('Digital Weight Scale', 'Precision digital scale for accurate weight measurement', 34.99, 40, 4, 15),
            ('Smart Plugs Set', 'Set of smart plugs for home automation', 49.99, 50, 4, 8),
            ('Digital Food Scale', 'Compact digital scale for precise kitchen measurements', 24.99, 25, 4, 15),

        -- Products for Sleeping Aids
            ('Memory Foam Pillow', 'Ergonomic memory foam pillow for a restful sleep', 49.99, 15, 5, 3),
            ('White Noise Machine', 'Portable white noise machine for better sleep', 39.99, 20, 5, 8),
            ('Sleep Tracker Watch', 'Smartwatch with sleep tracking features', 129.99, 10, 5, 4),
            ('Adjustable Bed Base', 'Electric bed base for customizable sleeping positions', 799.99, 5, 5, 14),
            ('Aromatherapy Diffuser', 'Essential oil diffuser for a calming sleep environment', 29.99, 30, 5, 6),

        -- Products for Audio and Video Equipment
            ('4K Gaming Monitor', 'Ultra HD monitor for an immersive gaming experience', 499.99, 10, 6, 26),
            ('Surround Sound System', 'High-quality surround sound system for home entertainment', 599.99, 15, 6, 27),
            ('Wireless Headphones', 'Over-ear wireless headphones for audio enthusiasts', 149.99, 25, 6, 28),
            ('Smart Projector', 'Portable smart projector for on-the-go entertainment', 349.99, 20, 6, 29),
            ('Bluetooth Soundbar', 'Compact soundbar for enhanced TV audio', 79.99, 30, 6, 30),

        -- Products for Climate Control Equipment
            ('Smart Thermostat', 'Intelligent thermostat for precise climate control', 129.99, 15, 7, 2),
            ('Air Purifier', 'High-efficiency air purifier for clean indoor air', 199.99, 10, 7, 1),
            ('Humidifier Plus', 'Advanced humidifier with smart humidity control', 79.99, 20, 7, 7),
            ('Tower Fan', 'Slim tower fan with multiple speed settings', 59.99, 25, 7, 6),
            ('Portable Heater', 'Compact portable heater for on-the-go warmth', 49.99, 30, 7, 2),

        -- Products for Gadgets for Kitchen
            ('Digital Meat Thermometer', 'Instant-read digital thermometer for cooking precision', 19.99, 40, 8, 15),
            ('Vegetable Spiralizer', 'Handheld spiralizer for creative vegetable dishes', 24.99, 35, 8, 5),
            ('Electric Salt and Pepper Grinder Set', 'Automated grinder set for convenient seasoning', 29.99, 30, 8, 8),
            ('Smart Food Scale', 'Bluetooth-enabled food scale for smart kitchen integration', 39.99, 25, 8, 12),
            ('Digital Tea Kettle', 'Programmable electric kettle for perfect tea brewing', 49.99, 20, 8, 12),

        -- Products for Lighting Equipment
            ('Smart LED Bulbs Set', 'Color-changing LED bulbs with smart home compatibility', 39.99, 15, 9, 4),
            ('Floor Lamp with Adjustable Brightness', 'Elegant floor lamp with customizable brightness levels', 79.99, 10, 9, 7),
            ('Under-Cabinet Lighting Kit', 'LED lighting kit for under-cabinet illumination', 29.99, 20, 9, 9),
            ('Smart Table Lamp', 'Table lamp with built-in smart features for convenience', 49.99, 25, 9, 10),
            ('Outdoor Solar Lights Set', 'Solar-powered lights for outdoor spaces', 69.99, 30, 9, 14),

        -- Products for Group Exercise Equipment
            ('Smart Treadmill', 'Connected treadmill with virtual training programs', 799.99, 10, 10, 2),
            ('Digital Rowing Machine', 'High-tech rowing machine for full-body workouts', 699.99, 15, 10, 4),
            ('Multi-Function Home Gym', 'Versatile home gym system for various exercises', 999.99, 8, 10, 8),
            ('Yoga Mat Set', 'Premium yoga mat set with accessories', 49.99, 20, 10, 3),
            ('Smart Fitness Tracker', 'Wearable fitness tracker with heart rate monitoring', 129.99, 25, 10, 2),

        -- Products for Bathroom Equipment
            ('Smart Shower Head', 'High-tech shower head with customizable settings', 79.99, 15, 11, 2),
            ('Digital Bathroom Scale', 'Modern bathroom scale with digital display', 34.99, 20, 11, 10),
            ('Touchless Soap Dispenser', 'Automatic soap dispenser for hands-free hygiene', 19.99, 25, 11, 11),
            ('Heated Towel Rack', 'Electric towel rack for warm towels after a shower', 129.99, 10, 11, 7),
            ('LED Vanity Mirror', 'Vanity mirror with built-in LED lights for optimal visibility', 49.99, 30, 11, 4),

        -- Products for Pet Care Equipment
            ('Automatic Pet Feeder', 'Smart pet feeder with scheduled feeding times', 59.99, 15, 12, 10),
            ('Pet Grooming Kit', 'Complete grooming kit for pets with multiple attachments', 49.99, 20, 12, 8),
            ('Interactive Cat Toy', 'Electronic toy for interactive play with cats', 24.99, 25, 12, 11),
            ('Dog Training Collar', 'Remote-controlled collar for training dogs', 39.99, 10, 12, 2),
            ('Pet GPS Tracker', 'GPS tracker for keeping tabs on your pet''s location', 79.99, 30, 12, 1),

        -- Products for Document Processing Equipment
            ('High-Speed Document Scanner', 'Fast scanner for efficient document digitization', 199.99, 15, 13, 3),
            ('Wireless Laser Printer', 'Wireless printer with laser technology for sharp prints', 129.99, 20, 13, 8),
            ('Digital Notepad', 'Smart notepad for digital note-taking and organization', 79.99, 25, 13, 9),
            ('Office Shredder', 'Heavy-duty shredder for secure document disposal', 149.99, 10, 13, 11),
            ('Document Binding Machine', 'Machine for professional document binding', 89.99, 30, 13, 14);
    )"
    );
}
