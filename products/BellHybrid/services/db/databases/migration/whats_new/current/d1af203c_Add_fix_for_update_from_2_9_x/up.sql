-- Copyright (c) 2017-2025, Mudita Sp. z.o.o. All rights reserved.
-- For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

-- Message: Add fix for update from 2 9 x
-- Revision: d1af203c-2982-4986-b680-fc7465019c4a
-- Create Date: 2025-08-07 17:30:39

-- Insert SQL here

INSERT OR REPLACE INTO whats_new (Major, Minor, Patch, English_title, Deutsch_title, Español_title, Polski_title, Français_title, Icon, English_desc, Deutsch_desc, Español_desc, Polski_desc, Français_desc) VALUES
    (2, 10, 0, 'Custom quotations', 'Eigene Zitate', 'Citas propias', 'Własne cytaty', 'Citations persos', 'whatsnew_quote_W_M', 'You can now add your own quotations using Mudita Center.', 'Füge Zitate mit Mudita Center hinzu.', 'Ahora puedes añadir tus propias citas usando Mudita Center.', 'Możesz teraz dodawać własne cytaty za pomocą Mudita Center.', 'Ajoutez vos citations via Mudita Center.');
