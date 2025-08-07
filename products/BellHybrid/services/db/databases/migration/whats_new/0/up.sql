-- Copyright (c) 2017-2025, Mudita Sp. z.o.o. All rights reserved.
-- For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

-- Create the database
CREATE TABLE IF NOT EXISTS whats_new (
    Major INTEGER NOT NULL,
    Minor INTEGER NOT NULL,
    Patch INTEGER NOT NULL,
    English_title TEXT NOT NULL,
    Deutsch_title TEXT NOT NULL,
    Español_title TEXT NOT NULL,
    Polski_title TEXT NOT NULL,
    Français_title TEXT NOT NULL,
    Icon TEXT NOT NULL,
    English_desc TEXT NOT NULL,
    Deutsch_desc TEXT NOT NULL,
    Español_desc TEXT NOT NULL,
    Polski_desc TEXT NOT NULL,
    Français_desc TEXT NOT NULL
);

-- Fill the database
INSERT OR REPLACE INTO whats_new (Major, Minor, Patch, English_title, Deutsch_title, Español_title, Polski_title, Français_title, Icon, English_desc, Deutsch_desc, Español_desc, Polski_desc, Français_desc) VALUES
    (2, 0, 0, 'Connection Status', 'Verbindungsstatus', 'Est. de conexión', 'Status połączenia', 'État de connexion', 'whatsnew_info_W_M', 'Check if Harmony is connected directly in Mudita Center.', 'Prüfe, ob dein Harmony direkt mit Mudita Center verbunden ist.', 'Comprueba si tu Harmony está conectado con el Mudita Center.', 'Sprawdź, czy Harmony jest połączone w Mudita Center.', 'Vérifiez directement la connexion de Harmony dans le Mudita Center.'),
    (2, 1, 0, 'Device Details', 'Gerätedaten', 'Info. dispositivo', 'Szczegóły urządz.', 'Détails appareil', 'whatsnew_info_W_M', 'Find the Serial Number and more in "About Device".', 'Seriennummer und mehr Infos unter "Über dieses Gerät".', 'Podrás ver el número de serie y más info. en Sobre el dispositivo.', 'Znajdź numer seryjny i nie tylko w sekcji "O urządzeniu".', 'Trouvez le No de série et plus dans "À propos de...".'),
    (2, 3, 0, 'Colors of Noise', 'Farbiges Rauschen', 'Ruidos coloridos', 'Kolory szumów', 'Bruits colorés', 'whatsnew_alarm_W_M', 'Find more ways to relax with "Colors of Noise" sounds.', 'Finde dank farbigem Rauschen neue Arten, dich zu entspannen.', 'Descubre nuevas formas de relax con los Ruidos coloridos.', 'Znajdź więcej sposobów na relaks z dźwiękami "Kolory szumów".', 'Détendez-vous autrement avec les "bruits colorés".'),
    (2, 3, 0, 'Light Fade-in', 'Lichteinblendung', 'Luz gradual', 'Zmienne podświetl.', 'Lumière graduelle', 'whatsnew_light_W_M', 'Wake up more gently with a fade-in backlight.', 'Sanftes Wecken mit allmählichem Anzeigelicht.', 'Ten un despertar más agradable con una iluminación gradual.', 'Budź się łagodniej z rosnącą jasnością podświetlenia.', 'Réveillez-vous en douceur grâce à un rétroéclairage progressif.'),
    (2, 4, 0, 'Alarm Fade-in', 'Wecker-Fade-in', 'Alarma gradual', 'Zmienna gł. alarmu', 'Alarme graduelle', 'whatsnew_alarm_W_M', 'Wake up more gently with a gradual increase in alarm volume.', 'Sanftes Wecken mit einem langsam lauter werdenden Ton.', 'Ten un despertar más agradable con una alarma gradual. ', 'Budź się łagodniej z narastającym dźwiękiem budzika.', 'Réveil doux par la hausse graduelle du vol. d''alarme.'),
    (2, 4, 0, 'More volume levels', 'Mehr Lautst.-Pegel', '+ niveles volumen', 'Więcej poz. głośn.', 'Plus de volumes', 'whatsnew_alarm_W_M', 'Select from both louder and quieter volume levels.', 'Wähle zwischen höheren und niedrigeren Lautstärkepegeln.', 'Escoge entre mayor o menor volumen.', 'Wybierz niższy lub wyższy poziom głośności.', 'Sélectionnez des volumes sonores élevés ou faibles.'),
    (2, 5, 0, 'Battery Warning', 'Akkuwarnhinweis', 'Aviso de batería', 'Ostrzeż. dot. bat.', 'Alerte de batterie', 'whatsnew_info_W_M', 'Get a helpful warning when it''s time to recharge.', 'Erhalte einen Warnhinweis, wenn es Zeit ist, den Akku zu laden.', 'Recibe un aviso cuando toque cargar la batería.', 'Uzyskaj pomocne ostrzeżenie o poziomie naładowania baterii.', 'Recevez une alerte lorsqu''il faut recharger l''appareil.'),
    (2, 6, 0, 'Quotations', 'Zitate', 'Citas', 'Cytaty', 'Citations', 'whatsnew_quote_W_M', 'Find inspiration with a selection of famous quotations.', 'Lass dich von einer Auswahl berühmter Zitate inspirieren.', 'Encuentra la inspiración con estas citas.', 'Znajdź inspirację dzięki znanym cytatom.', 'Trouvez l''inspiration grâce à une sélection de célèbres citations.'),
    (2, 7, 0, 'Focus Timer', 'Konzent.-Timer', 'Tpo. concentración', 'Timer skupienia', 'Minuteur concen.', 'whatsnew_timer_W_M', 'A series of focus times and breaks to help with productivity.', 'Konzentrationszeiten und Pausen für mehr Produktivität.', 'Temp. de concentración y descansos para productividad.', 'Seria chwil skupienia oraz przerw wspomagających produktywność.', 'Sessions de médit. et de pause pour une produc. accrue.'),
    (2, 8, 0, 'Custom Sounds', 'Eigene Klänge', 'Sonidos pers.', 'Własne dźwięki', 'Sons personnalisés', 'whatsnew_alarm_W_M', 'You can now upload sounds and use them as your main alarm.', 'Du kannst jetzt Klänge hochladen und diese als Hauptalarm verwenden.', 'Ahora puedes añadir tus sonidos para usarlos como tu alarma principal.', 'Możesz teraz przesyłać dźwięki i ustawiać je jako główny sygnał budzika.', 'Vous pouvez télécharger des sons et les utiliser comme alarme principale.'),
    (2, 7, 0, 'New meditation sound', 'Neuer Meditationston', 'Nuevos sonidos zen', 'Nowy ton medytacji', 'Nouv. son de médit.', 'whatsnew_alarm_W_M', 'End your meditation with a new sound.', 'Schließe deine Meditation mit einem neuen Klang ab.', 'Acaba tu meditación con un sonido nuevo.', 'Zakończ medytację nowym tonem.', 'Terminez votre méditation avec un nouveau son.'),
    (2, 9, 0, 'What''s new', 'Neues', 'Novedades', 'Nowości', 'Nouveautés', 'whatsnew_info_W_M', 'Discover features and changes with "What''s new" descriptions.', 'Entdecke Funktionen und Änderungen unter "Neues".', 'Accede a Novedades para ver todo lo nuevo.', 'Odkrywaj funkcje i zmiany dzięki opisom "Nowości".', 'Découvrez ce qui change grâce aux "Nouveautés".'),
    (2, 9, 0, 'Relaxation fade in', 'Entspannung graduell', 'Relajación gradual', 'Zmienne dźwięki', 'Fondu mode détente', 'whatsnew_alarm_W_M', 'Relaxation sounds now fade in and out smoothly.', 'Graduelles Ein- und Ausblenden von Entspannungsklängen.', 'Los sonidos comienzan y terminan gradualmente.', 'Płynne przejścia między dźwiękami relaksacyjnymi.', 'Un fondu est désormais appliqué aux sons de détente.');
