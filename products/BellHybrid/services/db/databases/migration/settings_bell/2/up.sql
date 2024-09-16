-- Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
-- For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

-- Message: Change tone names to tone paths
-- Revision: 1b034c28-f8f4-4792-8f35-9ab376fec0de
-- Create Date: 2024-03-06 15:07:49

-- Update snooze tone data
UPDATE OR IGNORE settings_tab
SET path = 'snooze_tone_path'
WHERE path = 'snooze_tone';

UPDATE OR IGNORE settings_tab
SET value = (
    CASE
    WHEN (SELECT value FROM settings_tab WHERE path = 'snooze_tone_path') = 'Blissful Dream'
    THEN '/system/assets/audio/chimes/Blissful_Dream.mp3'
    WHEN (SELECT value FROM settings_tab WHERE path = 'snooze_tone_path') = 'Gentle Chime'
    THEN '/system/assets/audio/chimes/Gentle_Chime.mp3'
    WHEN (SELECT value FROM settings_tab WHERE path = 'snooze_tone_path') = 'Rise & Shine'
    THEN '/system/assets/audio/chimes/Rise_&_Shine.mp3'
    WHEN (SELECT value FROM settings_tab WHERE path = 'snooze_tone_path') = 'Twinkle Chime'
    THEN '/system/assets/audio/chimes/Twinkle_Chime.mp3'
    END
    )
WHERE path = 'snooze_tone_path';

-- Update prewake up tone data
UPDATE OR IGNORE settings_tab
SET path = 'prewake_up_tone_path'
WHERE path = 'prewake_up_tone';

UPDATE OR IGNORE settings_tab
SET value = (
    CASE
    WHEN (SELECT value FROM settings_tab WHERE path = 'prewake_up_tone_path') = 'Joyful Awakening'
    THEN '/system/assets/audio/prewakeup/Joyful_Awakening.mp3'
    WHEN (SELECT value FROM settings_tab WHERE path = 'prewake_up_tone_path') = 'Morning Spirit'
    THEN '/system/assets/audio/prewakeup/Morning_Spirit.mp3'
    WHEN (SELECT value FROM settings_tab WHERE path = 'prewake_up_tone_path') = 'Radiant Morning'
    THEN '/system/assets/audio/prewakeup/Radiant_Morning.mp3'
    WHEN (SELECT value FROM settings_tab WHERE path = 'prewake_up_tone_path') = 'Spring Sunrise'
    THEN '/system/assets/audio/prewakeup/Spring_Sunrise.mp3'
    END
    )
WHERE path = 'prewake_up_tone_path';

-- Update alarm tone data
UPDATE OR IGNORE settings_tab
SET path = 'alarm_tone_path'
WHERE path = 'alarm_tone';

UPDATE OR IGNORE settings_tab
SET value = (
    CASE
    WHEN (SELECT value FROM settings_tab WHERE path = 'alarm_tone_path') = 'Morning Dew'
    THEN '/system/assets/audio/alarm/Morning_Dew.mp3'
    WHEN (SELECT value FROM settings_tab WHERE path = 'alarm_tone_path') = 'Tongue Drum'
    THEN '/system/assets/audio/alarm/Tongue_Drum.mp3'
    WHEN (SELECT value FROM settings_tab WHERE path = 'alarm_tone_path') = 'Koshi Dream'
    THEN '/system/assets/audio/alarm/Koshi_Dream.mp3'
    WHEN (SELECT value FROM settings_tab WHERE path = 'alarm_tone_path') = 'Tibetan Bowls'
    THEN '/system/assets/audio/alarm/Tibetan_Bowls.mp3'
    WHEN (SELECT value FROM settings_tab WHERE path = 'alarm_tone_path') = 'Reverie Harp'
    THEN '/system/assets/audio/alarm/Reverie_Harp.mp3'
    WHEN (SELECT value FROM settings_tab WHERE path = 'alarm_tone_path') = 'Tranquil Rainstick'
    THEN '/system/assets/audio/alarm/Tranquil_Rainstick.mp3'
    WHEN (SELECT value FROM settings_tab WHERE path = 'alarm_tone_path') = 'Hawaiian Blues'
    THEN '/system/assets/audio/alarm/Hawaiian_Blues.mp3'
    WHEN (SELECT value FROM settings_tab WHERE path = 'alarm_tone_path') = 'Sunrise Guitar'
    THEN '/system/assets/audio/alarm/Sunrise_Guitar.mp3'
    WHEN (SELECT value FROM settings_tab WHERE path = 'alarm_tone_path') = 'Acoustic Vibes'
    THEN '/system/assets/audio/alarm/Acoustic_Vibes.mp3'
    WHEN (SELECT value FROM settings_tab WHERE path = 'alarm_tone_path') = 'Back Porch'
    THEN '/system/assets/audio/alarm/Back_Porch.mp3'
    WHEN (SELECT value FROM settings_tab WHERE path = 'alarm_tone_path') = 'Cowboy Chords'
    THEN '/system/assets/audio/alarm/Cowboy_Chords.mp3'
    WHEN (SELECT value FROM settings_tab WHERE path = 'alarm_tone_path') = 'Bubbling Brook'
    THEN '/system/assets/audio/alarm/Bubbling_Brook.mp3'
    WHEN (SELECT value FROM settings_tab WHERE path = 'alarm_tone_path') = 'Autumnal Sea'
    THEN '/system/assets/audio/alarm/Autumnal_Sea.mp3'
    WHEN (SELECT value FROM settings_tab WHERE path = 'alarm_tone_path') = 'Nature''s Harmony'
    THEN '/system/assets/audio/alarm/Natures_Harmony.mp3'
    WHEN (SELECT value FROM settings_tab WHERE path = 'alarm_tone_path') = 'Delightful Morning'
    THEN '/system/assets/audio/alarm/Delightful_Morning.mp3'
    WHEN (SELECT value FROM settings_tab WHERE path = 'alarm_tone_path') = 'Always With You'
    THEN '/system/assets/audio/alarm/Always_With_You.mp3'
    WHEN (SELECT value FROM settings_tab WHERE path = 'alarm_tone_path') = 'Love, Peace and Harmony'
    THEN '/system/assets/audio/alarm/Love_Peace_and_Harmony.mp3'
    END
    )
WHERE path = 'alarm_tone_path';

-- Update bedtime tone data
UPDATE OR IGNORE settings_tab
SET path = 'bedtime_tone_path'
WHERE path = 'bedtime_tone';

UPDATE OR IGNORE settings_tab
SET value = (
    CASE
    WHEN (SELECT value FROM settings_tab WHERE path = 'bedtime_tone_path') = 'Evening Horizon'
    THEN '/system/assets/audio/evening_reminder/Evening_Horizon.mp3'
    WHEN (SELECT value FROM settings_tab WHERE path = 'bedtime_tone_path') = 'Evolving Dusk'
    THEN '/system/assets/audio/evening_reminder/Evolving_Dusk.mp3'
    WHEN (SELECT value FROM settings_tab WHERE path = 'bedtime_tone_path') = 'Melodic Mirth'
    THEN '/system/assets/audio/evening_reminder/Melodic_Mirth.mp3'
    WHEN (SELECT value FROM settings_tab WHERE path = 'bedtime_tone_path') = 'Twilight Gleam'
    THEN '/system/assets/audio/evening_reminder/Twilight_Gleam.mp3'
    END
    )
WHERE path = 'bedtime_tone_path';

-- Add fallback defaults for each tone
INSERT OR IGNORE INTO settings_tab (path, value) VALUES
   ('snooze_tone_path_default', '/system/assets/audio/chimes/Gentle_Chime.mp3'),
   ('prewake_up_tone_path_default', '/system/assets/audio/prewakeup/Joyful_Awakening.mp3'),
   ('alarm_tone_path_default', '/system/assets/audio/alarm/Morning_Dew.mp3'),
   ('bedtime_tone_path_default', '/system/assets/audio/evening_reminder/Evening_Horizon.mp3');

