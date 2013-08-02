-- Table: Track
CREATE TABLE IF NOT EXISTS Track ( 
    ID          INTEGER,
    Description TEXT    NOT NULL,
    Title       TEXT    NOT NULL,
    Distance    REAL    NOT NULL,
    Status      INTEGER NOT NULL,
    CONSTRAINT PK_Track PRIMARY KEY ( ID ) 
);

-- Table: Location
CREATE TABLE IF NOT EXISTS Location ( 
    ID        INTEGER,
    Latitude  REAL    NOT NULL,
    Longitude REAL    NOT NULL,
    Altitude  REAL,
    Timestamp TEXT    NOT NULL,
    Speed     REAL,
    Course    REAL,
    Track_ID  INTEGER,
    CONSTRAINT PK_Location PRIMARY KEY ( ID ),
    FOREIGN KEY ( Track_ID ) REFERENCES Track ( ID ) 
);

-- Table: POI
CREATE TABLE IF NOT EXISTS POI ( 
    Description    TEXT,
    Title          TEXT    NOT NULL,
    Latitude       REAL    NOT NULL,
    Longitude      REAL    NOT NULL,
    Altitude       REAL,
    Timestamp      TEXT    NOT NULL,
    ID             INTEGER,
    DefaultMediaID INTEGER,
    CONSTRAINT PK_POI PRIMARY KEY ( ID ) 
);


-- Table: Media
CREATE TABLE IF NOT EXISTS Media ( 
    ID        INTEGER,
    thumbnail BLOB    NOT NULL,
    POI_ID    INTEGER,
    source    TEXT    NOT NULL,
    CONSTRAINT PK_Media PRIMARY KEY ( ID ),
    FOREIGN KEY ( POI_ID ) REFERENCES POI ( ID ) 
);