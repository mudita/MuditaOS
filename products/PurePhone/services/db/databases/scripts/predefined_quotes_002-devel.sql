-- Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
-- For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md


-- To add new language please add here string exactly the same as desired display language
INSERT OR REPLACE INTO quote_languages (lang_id, lang_name) VALUES
    (1, 'Polski'),
    (2, 'English'),
    (3, 'Espanol'),
    (4, 'Francais'),
    (5, 'Deutsch');

INSERT OR REPLACE INTO category_table (category_id, lang_id, category_name) VALUES
    (1, 1, 'Kategoria 1'),
    (2, 1, 'Kategoria 2'),
    (3, 1, 'Kategoria 3'),
    (4, 2, 'Category 1'),
    (5, 2, 'Category 2'),
    (6, 2, 'Category 3'),
    (7, 3, 'Categoria 1'),
    (8, 3, 'Categoria 2'),
    (9, 3, 'Categoria 3'),
    (10, 4, 'Catégorie 1'),
    (11, 4, 'Catégorie 2'),
    (12, 4, 'Catégorie 3'),
    (13, 5, 'Kategorie 1'),
    (14, 5, 'Kategorie 2'),
    (15, 5, 'Kategorie 3');

-- To add new quote add here next consecutive key with quote and the author
-- Every quote has to be linked with specific one or many categories in quote_category_map
-- Every category has to be assigned to specific language in category_table
INSERT OR REPLACE INTO quote_table (quote_id, quote, author) VALUES
    (1,  'Quote 1 PL', 'Author 1 PL'),
    (2,  'Quote 2 PL', 'Author 2 PL'),
    (3,  'Quote 3 PL', 'Author 3 PL'),
    (4,  'Quote 4 PL', 'Author 4 PL'),
    (5,  'Quote 5 PL', 'Author 5 PL'),
    (6,  'Quote 1 EN', 'Author 1 EN'),
    (7,  'Quote 2 EN', 'Author 2 EN'),
    (8,  'Quote 3 EN', 'Author 3 EN'),
    (9,  'Quote 4 EN', 'Author 4 EN'),
    (10,  'Quote 5 EN', 'Author 5 EN'),
    (11,  'Quote 1 ES', 'Author 1 ES'),
    (12,  'Quote 2 ES', 'Author 2 ES'),
    (13,  'Quote 3 ES', 'Author 3 ES'),
    (14,  'Quote 4 ES', 'Author 4 ES'),
    (15,  'Quote 5 ES', 'Author 5 ES'),
    (16,  'Quote 1 FR', 'Author 1 FR'),
    (17,  'Quote 2 FR', 'Author 2 FR'),
    (18,  'Quote 3 FR', 'Author 3 FR'),
    (19,  'Quote 4 FR', 'Author 4 FR'),
    (20,  'Quote 5 FR', 'Author 5 FR'),
    (21,  'Quote 1 DE', 'Author 1 DE'),
    (22,  'Quote 2 DE', 'Author 2 DE'),
    (23,  'Quote 3 DE', 'Author 3 DE'),
    (24,  'Quote 4 DE', 'Author 4 DE'),
    (25,  'Quote 5 DE', 'Author 5 DE');
    
INSERT OR REPLACE INTO quote_category_map (category_id, quote_id) VALUES
    (1, 1), (2, 2), (3, 3), (1, 4), (2, 5), (3, 4), (3, 5),
    (4, 6), (5, 7), (6, 8), (4, 9), (5, 10), (6, 9), (6, 10),
    (7, 11), (8, 12), (9, 13), (7, 14), (8, 15), (9, 14), (9, 15),
    (10, 16), (11, 17), (12, 18), (10, 19), (11, 20), (12, 19), (12, 20),
    (13, 21), (14, 22), (15, 23), (13, 24), (14, 25), (15, 24), (15, 25);
