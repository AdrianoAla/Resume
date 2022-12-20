#include <cs50.h>
#include <stdio.h>
#include <string.h>

// Max number of candidates
#define MAX 9

// preferences[i][j] is number of voters who prefer i over j
int preferences[MAX][MAX];

// locked[i][j] means i is locked in over j
bool locked[MAX][MAX];

// Each pair has a winner, loser
typedef struct
{
    int winner;
    int loser;
}
pair;

// Array of candidates
string candidates[MAX];
pair pairs[MAX * (MAX - 1) / 2];

int pair_count;
int candidate_count;

// Function prototypes
bool vote(int rank, string name, int ranks[]);
void record_preferences(int ranks[]);
void add_pairs(void);
void sort_pairs(void);
void lock_pairs(void);
void print_winner(void);

int is_in_candidates(string s);

int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: tideman [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX)
    {
        printf("Maximum number of candidates is %i\n", MAX);
        return 2;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i] = argv[i + 1];
    }

    // Clear graph of locked in pairs
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            locked[i][j] = false;
        }
    }

    pair_count = 0;
    int voter_count = get_int("Number of voters: ");

    // Query for votes
    for (int i = 0; i < voter_count; i++)
    {
        // ranks[i] is voter's ith preference
        int ranks[candidate_count];

        // Query for each rank
        for (int j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);

            if (!vote(j, name, ranks))
            {
                printf("Invalid vote.\n");
                return 3;
            }
        }

        record_preferences(ranks);

        printf("\n");
    }

    add_pairs();
    sort_pairs();
    lock_pairs();
    print_winner();
    return 0;
}

int is_in_candidates(string s)
{
    for (int i = 0; i < candidate_count; i++)
    {
        if (strcmp(s, candidates[i]) == 0)
        {
            return i;
        }
    }

    return -1;
}

// Update ranks given a new vote
bool vote(int rank, string name, int ranks[])
{
    int index = is_in_candidates(name);
    if (index == -1)
    {
        return false;
    }
    ranks[rank] = index;
    return true;
}

// Update preferences given one voter's ranks
void record_preferences(int ranks[])
{
    for (int i = 0; i < candidate_count; i++)
    {
        int candidate = ranks[i];
        if (i + 1 == candidate_count)
        {
            continue;
        }

        for (int j = i + 1; j < candidate_count; j++)
        {
            int second_candidate = ranks[j];

            preferences[candidate][second_candidate] ++;

        }
    }
    return;
}

// Record pairs of candidates where one is preferred over the other
void add_pairs(void)
{
    int index = 0;

    for (int i = 0; i < candidate_count; i++)
    {
        if (i == candidate_count - 1)
        {
            continue;
        }

        for (int j = i + 1; j < candidate_count; j++)
        {
            if (preferences[i][j] == preferences[j][i])
            {
                continue;
            }

            else if (preferences[i][j] < preferences[j][i])
            {
                pairs[index].winner = j;
                pairs[index].loser = i;
            }

            else
            {
                pairs[index].winner = i;
                pairs[index].loser = j;
            }

            index++;
        }
    }
    pair_count = index;
    return;
}

// Sort pairs in decreasing order by strength of victory
void sort_pairs(void)
{
    for (int i = 0; i < pair_count; i++)
    {
        for (int j = 0; j < pair_count - 1; j++)
        {
            int margin_one = preferences[pairs[j].winner][pairs[j].loser] - preferences[pairs[j].loser][pairs[j].winner];
            int margin_two = preferences[pairs[j + 1].winner][pairs[j + 1].loser] - preferences[pairs[j + 1].loser][pairs[j + 1].winner];
            if (margin_one < margin_two)
            {
                pair temp = pairs[j];
                pairs[j] = pairs[j + 1];
                pairs[j + 1] = temp;
            }
        }
    }
    return;
}

bool is_cycle(int searching, int origin)
{
    for (int i = 0; i < candidate_count; i++)
    {
        if (locked[i][origin])
        {
            if (i == searching)
            {
                return true;
            }
            return is_cycle(searching, i);
        }
    }
    return false;
}

// Lock pairs into the candidate graph in order, without creating cycles
void lock_pairs(void)
{
    for (int i = 0; i < pair_count; i++)
    {
        if (!is_cycle(pairs[i].loser, pairs[i].winner))
        {
            locked[pairs[i].winner][pairs[i].loser] = true;
        }
    }

    return;
}

// Print the winner of the election
void print_winner(void)
{
    int tally[candidate_count];
    for (int i = 0; i < candidate_count; i++)
    {
        tally[i] = 0;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            if (locked[i][j])
            {
                tally[j] ++;
            }
        }
    }

    for (int i = 0; i < candidate_count; i++)
    {
        if (tally[i] == 0)
        {
            printf("%s\n", candidates[i]);
            return;
        }
    }
    return;
}
