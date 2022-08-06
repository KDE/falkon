-- Falkon browsedata.db

-- Tables
CREATE TABLE autofill (
    id INTEGER PRIMARY KEY,
    server TEXT NOT NULL,
    data TEXT,
    password TEXT,
    username TEXT,
    last_used INTEGER DEFAULT 0 NOT NULL
);
CREATE INDEX autofill_serverindex ON autofill (server);

CREATE TABLE autofill_encrypted (
    id INTEGER PRIMARY KEY,
    server TEXT NOT NULL,
    data_encrypted TEXT,
    password_encrypted TEXT,
    username_encrypted TEXT,
    last_used INTEGER DEFAULT 0 NOT NULL
);
CREATE INDEX autofill_encrypted_serverindex ON autofill_encrypted (server);

CREATE TABLE autofill_exceptions (
    id INTEGER PRIMARY KEY,
    server TEXT NOT NULL
);
CREATE INDEX autofill_exception_serverindex ON autofill_exceptions (server);

CREATE TABLE history (
    id INTEGER PRIMARY KEY,
    url TEXT NOT NULL,
    title TEXT,
    date INTEGER DEFAULT 0 NOT NULL,
    count INTEGER DEFAULT 0 NOT NULL
);
CREATE INDEX history_titleindex ON history (title);
CREATE UNIQUE INDEX history_urluniqueindex ON history (url);

CREATE TABLE search_engines (
    id INTEGER PRIMARY KEY,
    name TEXT NOT NULL,
    url TEXT,
    icon BLOB,
    shortcut TEXT,
    suggestionsUrl TEXT,
    suggestionsParameters TEXT,
    postData TEXT
);

CREATE TABLE icons (
    id INTEGER PRIMARY KEY,
    url TEXT NOT NULL,
    icon BLOB
);
CREATE UNIQUE INDEX icons_urluniqueindex ON icons (url);

CREATE TABLE site_settings (
    id INTEGER PRIMARY KEY,
    server TEXT NOT NULL,

    zoom_level INTEGER DEFAULT -1,
    allow_cookies INTEGER DEFAULT 0,

    wa_autoload_images INTEGER DEFAULT 0,
    wa_js_enabled INTEGER DEFAULT 0,
    wa_js_open_windows INTEGER DEFAULT 0,
    wa_js_access_clipboard INTEGER DEFAULT 0,
    wa_js_can_paste INTEGER DEFAULT 0,
    wa_js_window_activation INTEGER DEFAULT 0,
    wa_local_storage INTEGER DEFAULT 0,
    wa_fullscreen_support INTEGER DEFAULT 0,
    wa_run_insecure_content INTEGER DEFAULT 0,
    wa_playback_needs_gesture INTEGER DEFAULT 0,
    wa_reading_from_canvas INTEGER DEFAULT 0,
    wa_force_dark_mode INTEGER DEFAULT 0,

    f_notifications INTEGER DEFAULT 0,
    f_geolocation INTEGER DEFAULT 0,
    f_media_audio_capture INTEGER DEFAULT 0,
    f_media_video_capture INTEGER DEFAULT 0,
    f_media_audio_video_capture INTEGER DEFAULT 0,
    f_mouse_lock INTEGER DEFAULT 0,
    f_desktop_video_capture INTEGER DEFAULT 0,
    f_desktop_audio_video_capture INTEGER DEFAULT 0
);
CREATE UNIQUE INDEX site_settings_serveruniqueindex ON site_settings (server);

-- Data
