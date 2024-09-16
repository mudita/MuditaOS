-- Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
-- For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

-- Message: Change tone names to tone paths
-- Revision: 1b034c28-f8f4-4792-8f35-9ab376fec0de
-- Create Date: 2024-03-06 15:07:49

-- Revert snooze tone data update
UPDATE OR IGNORE settings_tab
SET value = (
    CASE
    WHEN (SELECT value FROM settings_tab WHERE path = 'snooze_tone_path') = '/system/assets/audio/chimes/Blissful_Dream.mp3'
    THEN 'Blissful Dream'
    WHEN (SELECT value FROM settings_tab WHERE path = 'snooze_tone_path') = '/system/assets/audio/chimes/Gentle_Chime.mp3'
    THEN 'Gentle Chime'
    WHEN (SELECT value FROM settings_tab WHERE path = 'snooze_tone_path') = '/system/assets/audio/chimes/Rise_&_Shine.mp3'
    THEN 'Rise & Shine'
    WHEN (SELECT value FROM settings_tab WHERE path = 'snooze_tone_path') =  '/system/assets/audio/chimes/Twinkle_Chime.mp3'
    THEN 'Twinkle Chime'
    END
    )
WHERE path = 'snooze_tone_path';

UPDATE OR IGNORE settings_tab
SET path = 'snooze_tone'
WHERE path = 'snooze_tone_path';

-- Revert prewake up tone data update
UPDATE OR IGNORE settings_tab
SET value = (
    CASE
    WHEN (SELECT value FROM settings_tab WHERE path = 'prewake_up_tone_path') =  '/system/assets/audio/prewakeup/Joyful_Awakening.mp3'
    THEN 'Joyful Awakening'
    WHEN (SELECT value FROM settings_tab WHERE path = 'prewake_up_tone_path') = '/system/assets/audio/prewakeup/Morning_Spirit.mp3'
    THEN 'Morning Spirit'
    WHEN (SELECT value FROM settings_tab WHERE path = 'prewake_up_tone_path') = '/system/assets/audio/prewakeup/Radiant_Morning.mp3'
    THEN 'Radiant Morning'
    WHEN (SELECT value FROM settings_tab WHERE path = 'prewake_up_tone_path') = '/system/assets/audio/prewakeup/Spring_Sunrise.mp3'
    THEN 'Spring Sunrise'
    END
    )
WHERE path = 'prewake_up_tone_path';

UPDATE OR IGNORE settings_tab
SET path = 'prewake_up_tone'
WHERE path = 'prewake_up_tone_path';

-- Revert alarm tone data update
UPDATE OR IGNORE settings_tab
SET value = (
    CASE
    WHEN (SELECT value FROM settings_tab WHERE path = 'alarm_tone_path') = '/system/assets/audio/alarm/Morning_Dew.mp3'
    THEN 'Morning Dew'
    WHEN (SELECT value FROM settings_tab WHERE path = 'alarm_tone_path') = '/system/assets/audio/alarm/Tongue_Drum.mp3'
    THEN 'Tongue Drum'
    WHEN (SELECT value FROM settings_tab WHERE path = 'alarm_tone_path') = '/system/assets/audio/alarm/Koshi_Dream.mp3'
    THEN 'Koshi Dream'
    WHEN (SELECT value FROM settings_tab WHERE path = 'alarm_tone_path') = '/system/assets/audio/alarm/Tibetan_Bowls.mp3'
    THEN 'Tibetan Bowls'
    WHEN (SELECT value FROM settings_tab WHERE path = 'alarm_tone_path') = '/system/assets/audio/alarm/Reverie_Harp.mp3'
    THEN 'Reverie Harp'
    WHEN (SELECT value FROM settings_tab WHERE path = 'alarm_tone_path') = '/system/assets/audio/alarm/Tranquil_Rainstick.mp3'
    THEN 'Tranquil Rainstick'
    WHEN (SELECT value FROM settings_tab WHERE path = 'alarm_tone_path') = '/system/assets/audio/alarm/Hawaiian_Blues.mp3'
    THEN 'Hawaiian Blues'
    WHEN (SELECT value FROM settings_tab WHERE path = 'alarm_tone_path') = '/system/assets/audio/alarm/Sunrise_Guitar.mp3'
    THEN 'Sunrise Guitar'
    WHEN (SELECT value FROM settings_tab WHERE path = 'alarm_tone_path') = '/system/assets/audio/alarm/Acoustic_Vibes.mp3'
    THEN 'Acoustic Vibes'
    WHEN (SELECT value FROM settings_tab WHERE path = 'alarm_tone_path') = '/system/assets/audio/alarm/Back_Porch.mp3'
    THEN 'Back Porch'
    WHEN (SELECT value FROM settings_tab WHERE path = 'alarm_tone_path') = '/system/assets/audio/alarm/Cowboy_Chords.mp3'
    THEN 'Cowboy Chords'
    WHEN (SELECT value FROM settings_tab WHERE path = 'alarm_tone_path') = '/system/assets/audio/alarm/Bubbling_Brook.mp3'
    THEN 'Bubbling Brook'
    WHEN (SELECT value FROM settings_tab WHERE path = 'alarm_tone_path') = '/system/assets/audio/alarm/Autumnal_Sea.mp3'
    THEN 'Autumnal Sea'
    WHEN (SELECT value FROM settings_tab WHERE path = 'alarm_tone_path') = '/system/assets/audio/alarm/Natures_Harmony.mp3'
    THEN 'Nature''s Harmony'
    WHEN (SELECT value FROM settings_tab WHERE path = 'alarm_tone_path') = '/system/assets/audio/alarm/Delightful_Morning.mp3'
    THEN 'Delightful Morning'
    WHEN (SELECT value FROM settings_tab WHERE path = 'alarm_tone_path') = '/system/assets/audio/alarm/Always_With_You.mp3'
    THEN 'Always With You'
    WHEN (SELECT value FROM settings_tab WHERE path = 'alarm_tone_path') = '/system/assets/audio/alarm/Love_Peace_and_Harmony.mp3'
    THEN 'Love, Peace and Harmony'
    END
    )
WHERE path = 'alarm_tone_path';

UPDATE OR IGNORE settings_tab
SET path = 'alarm_tone'
WHERE path = 'alarm_tone_path';

-- Revert bedtime tone data update
UPDATE OR IGNORE settings_tab
SET value = (
    CASE
    WHEN (SELECT value FROM settings_tab WHERE path = 'bedtime_tone_path') = '/system/assets/audio/evening_reminder/Evening_Horizon.mp3'
    THEN 'Evening Horizon'
    WHEN (SELECT value FROM settings_tab WHERE path = 'bedtime_tone_path') = '/system/assets/audio/evening_reminder/Evolving_Dusk.mp3'
    THEN 'Evolving Dusk'
    WHEN (SELECT value FROM settings_tab WHERE path = 'bedtime_tone_path') = '/system/assets/audio/evening_reminder/Melodic_Mirth.mp3'
    THEN 'Melodic Mirth'
    WHEN (SELECT value FROM settings_tab WHERE path = 'bedtime_tone_path') = '/system/assets/audio/evening_reminder/Twilight_Gleam.mp3'
    THEN 'Twilight Gleam'
    END
    )
WHERE path = 'bedtime_tone_path';

UPDATE OR IGNORE settings_tab
SET path = 'bedtime_tone'
WHERE path = 'bedtime_tone_path';

-- Remove fallback defaults
DELETE FROM settings_tab WHERE path = 'snooze_tone_path_default';
DELETE FROM settings_tab WHERE path = 'prewake_up_tone_path_default';
DELETE FROM settings_tab WHERE path = 'alarm_tone_path_default';
DELETE FROM settings_tab WHERE path = 'bedtime_tone_path_default';

